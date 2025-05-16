function allowAccess() {
  localStorage.setItem("allowRouteJump", "true");
}

function denyAccess() {
  localStorage.removeItem("allowRouteJump");
}

function hasAccess() {
  return localStorage.getItem("allowRouteJump") === "true";
}

function checkRouteAccess(targetRoute) {
  if (!hasAccess()) {
    if (window.location.pathname !== "/") {
      history.replaceState(null, "", "/");
      window.location.href = "/";
    }
    return false;
  }
  return true;
}
