/**
 * Permite el acceso a ciertas rutas configurando la bandera "allowRouteJump"
 * tanto en localStorage como en una cookie con path raíz.
 */
function allowAccess() {
  localStorage.setItem("allowRouteJump", "true");
  document.cookie = "allowRouteJump=true; path=/";
}

/**
 * Revoca el acceso eliminando la bandera "allowRouteJump"
 * tanto de localStorage como de la cookie, estableciendo Max-Age=0 para borrarla.
 */
function denyAccess() {
  localStorage.removeItem("allowRouteJump");
  document.cookie = "allowRouteJump=; Max-Age=0; path=/";
}

/**
 * Verifica si la bandera "allowRouteJump" está activa en localStorage,
 * lo que indica si el acceso está permitido.
 *
 * @returns {boolean} true si el acceso está permitido, false en caso contrario.
 */
function hasAccess() {
  return localStorage.getItem("allowRouteJump") === "true";
}

/**
 * Obtiene el entorno actual haciendo una solicitud fetch a "/environment/"
 * y retornando el valor del campo "environment" en la respuesta JSON.
 *
 * @returns {Promise<string>} Promesa que resuelve en el nombre del entorno actual.
 */
async function getEnvironment() {
  const response = await fetch("/environment/");
  const data = await response.json();
  return data.environment;
}

/**
 * Verifica si se permite el acceso a una ruta objetivo según el entorno y
 * la bandera de acceso almacenada. En entorno "dev" el acceso es libre.
 * Si no hay acceso, redirige a la ruta raíz ("/") y devuelve false.
 *
 * @param {string} targetRoute - La ruta a la que se quiere acceder.
 * @returns {Promise<boolean>} Promesa que resuelve en true si el acceso está permitido, false si no.
 */
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
