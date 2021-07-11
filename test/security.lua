local function insecurecall(func, ...)
  forceinsecure();
  local ok, err = pcall(func, ...);
  debug.forcesecure();

  if not ok then
    error(err, 0);
  end
end

local function errorhandler(err)
  print(err);
end

seterrorhandler(errorhandler);

--[[

  Constant Field Assignments

  The constants referenced by a function prototype inherit the security of
  the Lua context at the time of the prototype being parsed. Invoking a
  closure insecurely will, as a result, cause these constants to always
  be assigned securely to any passed in table.

  For example, assume the following function is defined:

    function SetFoo(t)
      t.foo = 100;
    end

  If the above is loaded as part of a secure script then the constant "100"
  is stored in the constant table of the parsed function prototype for SetFoo,
  which is referenced by the Lua closure assigned to the global of the same
  name.

  If SetFoo is then called *insecurely* with a table `t`, the resulting
  assignment of the constant "100" to the key `foo` doesn't actually taint
  since the taint status of the constant at the time it was loaded was secure.

  This can be seen ingame through the RectangleMixin, the following snippet
  will not trigger any assertions:

     local rect = CreateRectangle();
     assert(rect.left == 0, "expected 'rect.left' to be zero");
     assert(not issecurevariable(rect, "left"), "expected 'rect.left' to be insecure");
     rect:Reset();
     assert(rect.left == 0, "expected 'rect.left' to be zero");
     assert(issecurevariable(rect, "left"), "expected 'rect.left' to be secure");

  The initial assignment to `rect.left` ends up being insecure as the constant
  of "0.0" (in RectangleMixin:OnLoad()) is passed through a function, however
  when RectangleMixin:Reset() is called it directly assigns the constant 0.0
  to a field on `self` which results in a secure assignment.

--]]

local function TestConstantFieldAssignments(constant)
  if type(constant) == "string" then
    constant = string.format("%q", constant);
  else
    constant = tostring(constant);
  end

  -- Note that "Insecure" versus "Secure" below refers to the initial state
  -- of the context at the point when the string passed into loadstring was
  -- parsed and loaded. It doesn't reflect on what we're expecting in the
  -- results.

  local InsecureConstantTest = securecall(function() forceinsecure(); return assert(loadstring("(...).InsecureConstantTest = " .. constant)); end);
  local InsecureVariableTest = securecall(function() forceinsecure(); return assert(loadstring("(...).InsecureVariableTest = {}")); end);
  local SecureConstantTest = securecall(function() return assert(loadstring("forceinsecure(); (...).SecureConstantTest = " .. constant)); end);
  local SecureVariableTest = securecall(function() return assert(loadstring("forceinsecure(); (...).SecureVariableTest = {}")); end);

  assert(issecure(), "expected initial state to be secure");
  assert(type(InsecureConstantTest) == "function", "failed to load required function: InsecureConstantTest");
  assert(type(InsecureVariableTest) == "function", "failed to load required function: InsecureVariableTest");
  assert(type(SecureConstantTest) == "function", "failed to load required function: SecureConstantTest");
  assert(type(SecureVariableTest) == "function", "failed to load required function: SecureVariableTest");

  -- The below is necessarily for the test where constant is set to nil
  -- since the assignment to the below table would rehash and cause the
  -- InsecureConstantTest subtest to fail.

  local target = {
    InsecureConstantTest = nil,
    InsecureVariableTest = nil,
    SecureConstantTest = nil,
    SecureVariableTest = nil,
  };

  securecall(InsecureConstantTest, target);
  securecall(InsecureVariableTest, target);
  securecall(SecureConstantTest, target);
  securecall(SecureVariableTest, target);

  assert(issecure(), "expected state to remain secure");

  assert(not issecurevariable(target, "InsecureConstantTest"), "expected constant set by insecurely parsed prototype to be insecure");
  assert(not issecurevariable(target, "InsecureVariableTest"), "expected variable set by insecurely parsed prototype to be insecure");
  assert(issecurevariable(target, "SecureConstantTest"), "expected constant set by securely parsed prototype to be secure");
  assert(not issecurevariable(target, "SecureVariableTest"), "expected variable set by securely parsed prototype to be insecure");
end

TestConstantFieldAssignments("constant");
TestConstantFieldAssignments(12345);
TestConstantFieldAssignments(true);
TestConstantFieldAssignments(nil);

--[[

  Tainting Function Arguments

  As briefly touched on for the Constant Field Assignments, any would-be
  values or constants that are sent through functions where the execution
  either is or would become insecure should be immediately tainted.

  Assuming the following function pair is defined:

    function Bar(value)
        Baz = value;
    end

    function Foo()
      Bar(100);
    end

  Calling Foo insecurely shouldn't result in the global `Baz` being securely
  assigned. Similarly, calling Foo if Bar's closure was created insecurely
  should also not allow a secure assignment.

--]]

local function TestFunctionArguments(initial)
  local temp = {};

  temp.Bar1 = function(value) temp.baz1 = value; end
  temp.Bar2 = securecall(function() forceinsecure() return function(value) temp.baz2 = value; end; end);

  assert(issecure(), "expected initial state to be secure");
  assert(issecurevariable(temp, "Bar1"), "expected 'temp.Bar1' to be securely assigned");
  assert(issecurevariable(temp, "Bar2"), "expected 'temp.Bar2' to be securely assigned");

  insecurecall(function()
    forceinsecure();
    temp.Bar1(initial);
    assert(not issecurevariable(temp, "baz1"), "expected 'temp.baz1' to be insecurely assigned");
  end);

  insecurecall(function()
    temp.Bar2(initial);
    assert(not issecurevariable(temp, "baz2"), "expected 'temp.baz2' to be insecurely assigned");
  end);
end

TestFunctionArguments("constant");
TestFunctionArguments(12345);
TestFunctionArguments(true);
TestFunctionArguments(nil);
TestFunctionArguments({});
TestFunctionArguments(print);

--[[

  Tainting Function Returns

  As with the Tainting Function Arguments test, we expect that a secure
  closure returning a secure constant if invoked insecurely shouldn't allow
  the values returned by the function to keep their security.

    function Foo()
      return 100;
    end

  Assuming Foo was loaded securely the constant "100" will itself also be
  secure, however calling Foo insecurely _should_ taint the value returned
  from the function irrespective of it being a constant.

  This can be verified ingame through the following:

    A = ScrollBoxLinearBaseViewMixin.GetStride();
    assert(A == 1, "expected 'A' to be 1");
    assert(not issecurevariable("A"), "expected 'A' to be insecure");

--]]


local function TestFunctionReturns(initial)
  local temp = {};

  local function GetInitialValue()
    return initial;
  end

  assert(issecure(), "expected initial state to be secure");

  insecurecall(function()
    temp.initial = GetInitialValue();
    assert(not issecurevariable(temp, "initial"), "expected 'temp.initial' to be insecurely assigned");
  end);
end

TestFunctionReturns("constant");
TestFunctionReturns(12345);
TestFunctionReturns(true);
TestFunctionReturns(nil);
TestFunctionReturns({});
TestFunctionReturns(print);
