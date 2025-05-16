function allowAccess() {
  localStorage.setItem("allowRouteJump", "true");
  document.cookie = "allowRouteJump=true; path=/";
}

function denyAccess() {
  localStorage.removeItem("allowRouteJump");
  document.cookie = "allowRouteJump=; Max-Age=0; path=/";
}

function hasAccess() {
  return localStorage.getItem("allowRouteJump") === "true";
}

async function getEnvironment() {
  const response = await fetch("/environment/");
  const data = await response.json();
  return data.environment;
}

async function checkRouteAccess(targetRoute) {
  const env = await getEnvironment();
  const access = hasAccess();

  console.log(`[DEBUG] Entorno: ${env}`);
  console.log(`[DEBUG] hasAccess: ${access}`);
  console.log(`[DEBUG] Ruta actual: ${window.location.pathname}`);
  console.log(`[DEBUG] Ruta solicitada: ${targetRoute}`);

  if (env === "dev") {
    console.log("[DEBUG] Entorno de desarrollo, permitiendo acceso libre.");
    return true;
  }

  if (!access) {
    console.log("[DEBUG] Acceso denegado, redirigiendo a '/'.");
    if (window.location.pathname !== "/") {
      history.replaceState(null, "", "/");
      window.location.href = "/";
    }
    return false;
  }

  console.log("[DEBUG] Acceso permitido.");
  return true;
}
