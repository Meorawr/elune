-- This fixture may be loaded either securely or not.
function securecall_identity(...) return ...; end
function securecall_forceinsecure(...) forceinsecure(); return ...; end
function securecall_error(...) error(...); end
function securecall_insecureerror(...) forceinsecure(); error(...); end
