-- This script should be loaded securely.
assert(issecure());

local function pack(...)
  return { n = select("#", ...), ... };
end

function securecall_checked(func, ...)
  local err;
  local errorhandler = function(e) err = e; end;

  seterrorhandler(errorhandler);

  local results = pack(securecall(func, ...));

  if err ~= nil then
    error(err, 0);
  else
    return unpack(results, 1, results.n);
  end
end

function insecurecall_checked(func, ...)
  local wrapper = function(...)
    forceinsecure();
    return func(...);
  end

  return securecall_checked(wrapper, ...);
end

function identity(...)
  return ...;
end
