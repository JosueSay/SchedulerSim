async function initializeConfig() {
  const hasRouteAccess = await checkRouteAccess("/config");
  if (!hasRouteAccess) return;
}

function onAlgorithmChange() {
  const algorithm = document.getElementById("algorithm").value;
  document.getElementById("quantum-config").style.display =
    algorithm === "RR" ? "flex" : "none";
  document.getElementById("preemptive-config").style.display =
    algorithm === "PS" ? "flex" : "none";
}

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

function startSimulation() {
  const config = {
    algorithm: document.getElementById("algorithm").value,
    quantum: document.getElementById("quantum").value || null,
    isPreemptive: document.getElementById("isPreemptive").value,
    startCycle: parseInt(document.getElementById("startCycle").value),
    maxCycles: parseInt(document.getElementById("maxCycles").value),
  };

  if (!validateSimulationConfig(config)) return;

  console.log("[DEBUG] Configuración de Simulación:", config);
  showAlert(
    "Simulación Configurada",
    "Configuración completada. (Falta integrar con Backend)",
    "success"
  );
}
