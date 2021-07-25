

local function InsecureAddition()
  local A = identity(100);
  local B = identity(200);

  assert(not issecure(), "expected state to be insecure");
  assert(not debug.issecurelocal(1, 1), "expected 'A' to be insecure");
  assert(not debug.issecurelocal(1, 2), "expected 'B' to be insecure");

  local C = A + B;

  assert(not debug.issecurelocal(1, 3), "expected 'C' to be insecure");
end

local function InsecureSubtraction()
  local A = identity(100);
  local B = identity(200);

  assert(not issecure(), "expected state to be insecure");
  assert(not debug.issecurelocal(1, 1), "expected 'A' to be insecure");
  assert(not debug.issecurelocal(1, 2), "expected 'B' to be insecure");

  local C = A - B;

  assert(not debug.issecurelocal(1, 3), "expected 'C' to be insecure");
end

local function InsecureDivision()
  local A = identity(100);
  local B = identity(200);

  assert(not issecure(), "expected state to be insecure");
  assert(not debug.issecurelocal(1, 1), "expected 'A' to be insecure");
  assert(not debug.issecurelocal(1, 2), "expected 'B' to be insecure");

  local C = A / B;

  assert(not debug.issecurelocal(1, 3), "expected 'C' to be insecure");
end

local function InsecureMultiplication()
  local A = identity(100);
  local B = identity(200);

  assert(not issecure(), "expected state to be insecure");
  assert(not debug.issecurelocal(1, 1), "expected 'A' to be insecure");
  assert(not debug.issecurelocal(1, 2), "expected 'B' to be insecure");

  local C = A * B;

  assert(not debug.issecurelocal(1, 3), "expected 'C' to be insecure");
end

local function InsecureModulo()
  local A = identity(100);
  local B = identity(200);

  assert(not issecure(), "expected state to be insecure");
  assert(not debug.issecurelocal(1, 1), "expected 'A' to be insecure");
  assert(not debug.issecurelocal(1, 2), "expected 'B' to be insecure");

  local C = A % B;

  assert(not debug.issecurelocal(1, 3), "expected 'C' to be insecure");
end

local function InsecurePow()
  local A = identity(100);
  local B = identity(200);

  assert(not issecure(), "expected state to be insecure");
  assert(not debug.issecurelocal(1, 1), "expected 'A' to be insecure");
  assert(not debug.issecurelocal(1, 2), "expected 'B' to be insecure");

  local C = A ^ B;

  assert(not debug.issecurelocal(1, 3), "expected 'C' to be insecure");
end

local function InsecureUnaryMinus()
  local A = identity(100);

  assert(not issecure(), "expected state to be insecure");
  assert(not debug.issecurelocal(1, 1), "expected 'A' to be insecure");

  local B = -A;

  assert(not debug.issecurelocal(1, 2), "expected 'B' to be insecure");
end

insecurecall_checked(InsecureAddition);
insecurecall_checked(InsecureSubtraction);
insecurecall_checked(InsecureDivision);
insecurecall_checked(InsecureMultiplication);
insecurecall_checked(InsecureModulo);
insecurecall_checked(InsecurePow);
insecurecall_checked(InsecureUnaryMinus);
