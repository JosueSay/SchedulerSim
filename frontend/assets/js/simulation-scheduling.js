const cycleCounter = document.getElementById("cycle-counter");
const simulationStatus = document.getElementById("simulation-status");

const processColors = {};
let colorIndex = 0;
let currentCycle = 0;
const events = [];
const processMetrics = [];

/**
 * Genera un color distintivo basado en hue espaciado.
 * @param {number} index - índice único para espaciado
 * @returns {string} color en formato hsl
 */
function generateBaseRGBA(index) {
  const hue = (index * 137.508) % 360;
  return `hsla(${hue}, 65%, 55%,`;
}

/**
 * Retorna o asigna un color único a un proceso por su PID
 */
function getProcessColor(pid) {
  if (!processColors[pid]) {
    processColors[pid] = generateBaseRGBA(colorIndex++);
  }
  return processColors[pid]; // Devuelve por ejemplo "hsla(120,65%,55%,"
}

function getColorForProcess(pid, state) {
  const base = getProcessColor(pid); // devuelve algo como hsla(...,
  switch (state) {
    case "NEW":
    case "TERMINATED":
      return `${base} 1)`; // opacidad completa
    case "ACCESSED":
      return `${base} 0.7)`; // tono medio
    case "WAITING":
      return `${base} 0.3)`; // tono opaco
    default:
      return `${base} 1)`; // fallback
  }
}

/**
 * Renderiza la tabla Gantt con eventos de procesos.
 * Cada fila representa un proceso y cada columna un ciclo de tiempo.
 * Las celdas se colorean según el proceso que está activo en ese ciclo.
 * Además, se genera una leyenda con los colores asignados a cada proceso.
 *
 * @param {Array} events - Array de eventos con objetos que contienen:
 *   pid: identificador del proceso,
 *   startCycle: ciclo donde inicia el evento,
 *   endCycle: ciclo donde termina el evento.
 */
function renderGanttTable(events) {
  // Obtiene la referencia a la tabla y a la leyenda en el DOM
  const table = document.getElementById("gantt-table");
  const legend = document.getElementById("gantt-legend");

  // Limpia cualquier contenido previo en la tabla y la leyenda
  table.innerHTML = "";
  legend.innerHTML = "";

  const pids = [...new Set(events.map((e) => e.pid))];
  const maxCycle = Math.max(...events.map((e) => e.endCycle));

  // Construye la leyenda mostrando el color asignado a cada proceso (usando color de estado NEW)
  pids.forEach((pid) => {
    const legendColor = getColorForProcess(pid, "NEW");
    legend.innerHTML += `<div class="legend-item"><div class="color-box" style="background:${legendColor}"></div>${pid}</div>`;
  });

  // Construye la fila de encabezado con el título y números de ciclo
  let headerRow = "<tr><th>Proceso/Ciclo</th>";
  for (let c = 0; c <= maxCycle; c++) headerRow += `<th>${c}</th>`;
  headerRow += "</tr>";
  table.innerHTML += headerRow;

  // Construye cada fila para cada proceso
  pids.forEach((pid) => {
    let row = `<tr><td>${pid}</td>`;
    for (let c = 0; c <= maxCycle; c++) {
      // Obtener todos los eventos que coincidan con este ciclo y proceso
      const matchingEvents = events.filter(
        (e) => e.pid === pid && c >= e.startCycle && c < e.endCycle
      );

      // Prioridad: NEW y TERMINATED por encima de ACCESSED y WAITING
      let chosenEvent = null;
      const priorityOrder = ["NEW", "TERMINATED", "ACCESSED", "WAITING"];
      for (const state of priorityOrder) {
        chosenEvent = matchingEvents.find((e) => e.state === state);
        if (chosenEvent) break;
      }

      row += chosenEvent
        ? `<td style="background:${getColorForProcess(
            pid,
            chosenEvent.state
          )}; color: #000;">${pid}</td>`
        : "<td></td>";
    }
    row += "</tr>";
    table.innerHTML += row;
  });
}

function renderMetricsTable() {
  if (processMetrics.length === 0) return;

  const table = document.getElementById("metrics-table");
  const average = document.getElementById("metrics-average");

  table.innerHTML = `
    <thead>
      <tr>
        <th>PID</th>
        <th>AT</th>
        <th>BT</th>
        <th>Priority</th>
        <th>Start</th>
        <th>End</th>
        <th>Waiting</th>
      </tr>
    </thead>
    <tbody>
      ${processMetrics
        .map(
          (p) => `
        <tr>
          <td>${p.pid}</td>
          <td>${p.arrivalTime}</td>
          <td>${p.burstTime}</td>
          <td>${p.priority}</td>
          <td>${p.startTime}</td>
          <td>${p.endTime}</td>
          <td>${p.waitingTime}</td>
        </tr>
      `
        )
        .join("")}
    </tbody>
  `;

  const avgWaiting = (
    processMetrics.reduce((sum, p) => sum + p.waitingTime, 0) /
    processMetrics.length
  ).toFixed(2);

  average.textContent = `Average Waiting Time: ${avgWaiting} ciclos`;
}

/**
 * Inicializa la simulación y gestiona WebSocket
 */
let ws;
function initializeSimulation() {
  cycleCounter.textContent = "Ciclo Actual: --";
  simulationStatus.textContent = "Estado: Esperando configuración";
  document.getElementById("algorithm-used").textContent = "Algoritmo: --";
  processMetrics.length = 0;
  events.length = 0;

  const config = JSON.parse(localStorage.getItem("lastSimulationConfig")) || {
    algorithm: "FIFO",
  };

  ws = new WebSocket("ws://127.0.0.1:8000/ws/simulation-scheduling");

  ws.onopen = () => {
    console.log("Conexión WebSocket establecida.");
    console.log("Configuración de simulación:", config);
    ws.send(JSON.stringify(config));
    document.getElementById(
      "algorithm-used"
    ).textContent = `Algoritmo: ${config.algorithm}`;

    // mostrar quantum si aplica
    const quantumContainer = document.getElementById("quantum-info");
    if (config.algorithm === "RR" && config.quantum) {
      quantumContainer.textContent = `Quantum: ${config.quantum}`;
      quantumContainer.style.display = "block";
    } else {
      quantumContainer.style.display = "none";
    }

    // Mostrar info de preemptivo si aplica
    const preemptiveContainer = document.getElementById("preemptive-info");
    if (config.algorithm === "PS") {
      const label =
        config.isPreemptive === "1" || config.isPreemptive === 1 ? "Sí" : "No";
      preemptiveContainer.textContent = `Preemptivo: ${label}`;
      preemptiveContainer.style.display = "block";
    } else {
      preemptiveContainer.style.display = "none";
    }
  };

  ws.onmessage = (event) => {
    const data = JSON.parse(event.data);

    if (data.event === "SIMULATION_END") {
      ws.close();
      cycleCounter.textContent = `Ciclo Actual: ${currentCycle}`;
      simulationStatus.textContent = "Estado: Finalizado";
      renderMetricsTable();
      return;
    }

    if (data.event === "PROCESS_METRIC") {
      processMetrics.push(data);
    } else if (data.pid) {
      currentCycle = data.endCycle;
      events.push(data);
      renderGanttTable(events);
      cycleCounter.textContent = `Ciclo Actual: ${currentCycle}`;
      simulationStatus.textContent = "Estado: En proceso";
    }
  };

  ws.onerror = (err) => {
    console.error("Error en WebSocket:", err);
    showAlert("Error", "Conexión con la simulación perdida.", "error");
  };

  ws.onclose = () => {
    console.log("Conexión WebSocket cerrada.");
  };
}

/**
 * Reinicia la simulación limpiando tabla y ciclo,
 * cerrando y reiniciando WebSocket
 */
function resetSimulation() {
  cycleCounter.textContent = "Ciclo Actual: --";
  simulationStatus.textContent = "Estado: Reiniciado";

  document.getElementById("gantt-table").innerHTML = "";
  document.getElementById("gantt-legend").innerHTML = "";
  document.getElementById("metrics-panel").innerHTML = "";

  if (ws) ws.close();
  initializeSimulation();
}

/**
 * Navegación entre vistas
 */
function restartSimulation() {
  denyAccess();
  window.location.href = "/config-scheduling";
}

function returnHome() {
  denyAccess();
  window.location.href = "/";
}
