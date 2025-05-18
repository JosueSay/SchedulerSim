/**
 * Inicializa la configuración verificando acceso a la ruta "/config".
 * Si no se tiene acceso, no continúa.
 *
 * @returns {Promise<void>}
 */
async function initializeConfig() {
  const hasRouteAccess = await checkRouteAccess("/config");
  if (!hasRouteAccess) return;
}

/**
 * Maneja el cambio en el selector de algoritmo.
 * Muestra u oculta configuraciones específicas según el algoritmo seleccionado:
 * - Muestra configuración de quantum si es "RR" (Round Robin).
 * - Muestra configuración preemptiva si es "PS" (Priority Scheduling).
 */
function onAlgorithmChange() {
  const algorithm = document.getElementById("algorithm").value;
  document.getElementById("quantum-config").style.display =
    algorithm === "RR" ? "flex" : "none";
  document.getElementById("preemptive-config").style.display =
    algorithm === "PS" ? "flex" : "none";
}

/**
 * Valida la configuración de simulación ingresada.
 * Muestra alertas específicas si alguna condición no se cumple.
 *
 * @param {Object} config - Objeto con la configuración.
 * @param {string} config.algorithm - Algoritmo seleccionado.
 * @param {string|null} config.quantum - Valor del quantum, si aplica.
 * @param {string} config.isPreemptive - Valor que indica si es preemptivo.
 * @param {number} config.startCycle - Ciclo inicial de la simulación.
 * @param {number} config.maxCycles - Máximo de ciclos de la simulación.
 * @returns {boolean} true si la configuración es válida, false si no.
 */
function validateSimulationConfig(config) {
  if (config.startCycle < 0) {
    showAlert(
      "Error de Configuración",
      "El ciclo inicial no puede ser negativo.",
      "error"
    );
    return false;
  }

  if (config.maxCycles < 0) {
    showAlert(
      "Error de Configuración",
      "Los ciclos máximos no pueden ser negativos.",
      "error"
    );
    return false;
  }

  if (config.algorithm === "RR") {
    if (!config.quantum || parseInt(config.quantum) <= 0) {
      showAlert(
        "Error de Quantum",
        "El quantum debe ser un número positivo para Round Robin.",
        "warning"
      );
      return false;
    }
  }

  if (config.algorithm !== "PS" && config.isPreemptive !== "0") {
    showAlert(
      "Configuración Incorrecta",
      "El modo preventivo solo aplica para Priority Scheduling.",
      "info"
    );
    return false;
  }

  return true;
}

/**
 * Inicia la simulación leyendo la configuración del formulario,
 * validándola y, si es válida, la guarda en localStorage para uso posterior.
 * Luego redirige a la página de simulación.
 *
 * @returns {Promise<void>}
 */
async function startSimulation() {
  const config = {
    algorithm: document.getElementById("algorithm").value,
    quantum: document.getElementById("quantum").value || null,
    isPreemptive: document.getElementById("isPreemptive").value,
    startCycle: parseInt(document.getElementById("startCycle").value),
    maxCycles: parseInt(document.getElementById("maxCycles").value),
  };

  if (!validateSimulationConfig(config)) return;

  // Guardar configuración para uso posterior
  localStorage.setItem("lastSimulationConfig", JSON.stringify(config));
  window.location.href = "/simulation";
}
