const cycleCounter = document.getElementById("cycle-counter");
const simulationStatus = document.getElementById("simulation-status");
let sortGanttByPID = false; // false = orden original
let currentSortField = "";
let sortAscending = true;
const resourcesLoaded = [];
const actionsLoaded = [];
let mechanismUsed = "";

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
      return `${base} 1)`; // tono medio
    case "WAITING":
      return `${base} 0.3)`; // tono opaco
    default:
      return `${base} 1)`; // fallback
  }
}

function renderResourcesTable() {
  const table = document.getElementById("resources-table");
  table.innerHTML = `
    <thead>
      <tr>
        <th>Nombre</th>
        <th>Disponible</th>
      </tr>
    </thead>
    <tbody>
      ${resourcesLoaded
        .map(
          (r) => `
          <tr>
            <td>${r.name}</td>
            <td>${r.counter}</td>
          </tr>
        `
        )
        .join("")}
    </tbody>
  `;
}

function renderActionsTable() {
  const table = document.getElementById("actions-table");
  table.innerHTML = `
    <thead>
      <tr>
        <th>PID</th>
        <th>Acción</th>
        <th>Recurso</th>
        <th>Ciclo</th>
      </tr>
    </thead>
    <tbody>
      ${actionsLoaded
        .map(
          (a) => `
          <tr>
            <td>${a.pid}</td>
            <td>${a.action}</td>
            <td>${a.resource}</td>
            <td>${a.cycle}</td>
          </tr>
        `
        )
        .join("")}
    </tbody>
  `;
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
  const table = document.getElementById("gantt-table");
  const legend = document.getElementById("gantt-legend");

  table.innerHTML = "";
  legend.innerHTML = "";

  let pids = [...new Set(events.map((e) => e.pid))];
  if (sortGanttByPID) {
    pids.sort((a, b) => {
      const numA = a.match(/\d+/);
      const numB = b.match(/\d+/);
      if (numA && numB) return parseInt(numA[0]) - parseInt(numB[0]);
      return a.localeCompare(b);
    });
  }

  const maxCycle = Math.max(...events.map((e) => e.endCycle));

  // Leyenda
  pids.forEach((pid) => {
    const legendColor = getColorForProcess(pid, "NEW");
    legend.innerHTML += `<div class="legend-item"><div class="color-box" style="background:${legendColor}"></div>${pid}</div>`;
  });

  // Encabezado
  let headerRow = "<tr><th>Proceso/Ciclo</th>";
  for (let c = 0; c <= maxCycle; c++) headerRow += `<th>${c}</th>`;
  headerRow += "</tr>";
  table.innerHTML += headerRow;

  // Filas de procesos
  pids.forEach((pid) => {
    let row = `<tr><td>${pid}</td>`;
    for (let c = 0; c <= maxCycle; c++) {
      const matching = events.filter(
        (e) => e.pid === pid && c >= e.startCycle && c < e.endCycle
      );

      const hasNew = matching.some((e) => e.state === "NEW");
      const hasAccessed = matching.some((e) => e.state === "ACCESSED");
      const hasTerminated = matching.some((e) => e.state === "TERMINATED");
      const hasWaiting = matching.some((e) => e.state === "WAITING");

      let label = "";
      let color = "";

      if (hasTerminated) {
        label = `TD - ${pid}`;
        color = getColorForProcess(pid, "ACCESSED");
      } else if (hasAccessed && hasNew) {
        label = `NW - ${pid}`;
        color = getColorForProcess(pid, "ACCESSED");
      } else if (hasWaiting && hasNew) {
        label = `NW - ${pid}`;
        color = getColorForProcess(pid, "WAITING");
      } else if (hasAccessed) {
        label = pid;
        color = getColorForProcess(pid, "ACCESSED");
      } else if (hasWaiting) {
        label = "";
        color = getColorForProcess(pid, "WAITING");
      } else if (hasNew) {
        label = `NW - ${pid}`;
        color = "transparent";
      }

      row += `<td style="background:${color}; color:#000;">${label}</td>`;
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

    if (data.event === "CONFIG") {
      mechanismUsed = data.mechanism;
      document.getElementById(
        "mechanism-used"
      ).textContent = `Mecanismo: ${mechanismUsed}`;
    } else if (data.event === "RESOURCE_LOADED") {
      resourcesLoaded.push(data);
      renderResourcesTable();
    } else if (data.event === "ACTION_LOADED") {
      actionsLoaded.push(data);
      renderActionsTable();
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
function resetScheduling() {
  // Cierra WebSocket si aún está abierto
  if (ws && ws.readyState !== WebSocket.CLOSED) {
    ws.close();
  }

  // Limpiar estado visual
  cycleCounter.textContent = "Ciclo Actual: --";
  simulationStatus.textContent = "Estado: Reiniciado";
  document.getElementById("gantt-table").innerHTML = "";
  document.getElementById("gantt-legend").innerHTML = "";
  document.getElementById("metrics-table").innerHTML = "";
  document.getElementById("metrics-average").textContent = "";

  // Limpiar datos
  events.length = 0;
  processMetrics.length = 0;
  currentCycle = 0;
  colorIndex = 0;
  Object.keys(processColors).forEach((key) => delete processColors[key]);

  // Reiniciar la simulación
  initializeSimulation();
}

/**
 * Navegación entre vistas
 */
function returnConfigurationScheduling() {
  denyAccess();
  window.location.href = "/config-scheduling";
}

function returnConfigurationSync() {
  denyAccess();
  window.location.href = "/config-synchronization";
}

function returnHome() {
  denyAccess();
  window.location.href = "/";
}

function downloadScreenshot() {
  const element = document.getElementById("simulation-report");
  const ganttScroll = document.getElementById("gantt-scroll");
  const metricsScroll = document.getElementById("metrics-scroll");

  // Guardar estilos originales
  const originalReportStyle = element.style.cssText;
  const originalGanttStyle = ganttScroll.style.cssText;
  const originalMetricsStyle = metricsScroll.style.cssText;

  // Aplicar estilos temporales para expandir completamente
  element.style.width = "max-content";
  ganttScroll.style.maxHeight = "none";
  ganttScroll.style.overflow = "visible";
  metricsScroll.style.maxHeight = "none";
  metricsScroll.style.overflow = "visible";

  // Esperar a que se apliquen los estilos antes de capturar
  setTimeout(() => {
    html2canvas(element, {
      scale: 3,
      useCORS: true,
      scrollX: 0,
      scrollY: 0,
      windowWidth: document.body.scrollWidth,
      windowHeight: document.body.scrollHeight,
    }).then((canvas) => {
      const link = document.createElement("a");
      link.download = "simulacion_scheduling.png";
      link.href = canvas.toDataURL("image/png");
      link.click();

      // Restaurar estilos originales
      element.style.cssText = originalReportStyle;
      ganttScroll.style.cssText = originalGanttStyle;
      metricsScroll.style.cssText = originalMetricsStyle;
    });
  }, 100);
}

function toggleGanttOrder() {
  sortGanttByPID = !sortGanttByPID;
  renderGanttTable(events);

  const ganttBtn = document.querySelector(
    "button[onclick='toggleGanttOrder()']"
  );
  if (sortGanttByPID) {
    ganttBtn.classList.add("active");
  } else {
    ganttBtn.classList.remove("active");
    ganttBtn.textContent = "Ordenar por PID";
  }
}

function sortMetrics(field) {
  if (currentSortField === field) {
    sortAscending = !sortAscending;
  } else {
    currentSortField = field;
    sortAscending = true;
  }

  processMetrics.sort((a, b) => {
    if (typeof a[field] === "string") {
      return sortAscending
        ? a[field].localeCompare(b[field])
        : b[field].localeCompare(a[field]);
    } else {
      return sortAscending ? a[field] - b[field] : b[field] - a[field];
    }
  });

  renderMetricsTable();

  // Estilo activo
  document
    .querySelectorAll(".metrics-sort-buttons .secondary-button")
    .forEach((btn) => {
      btn.classList.remove("active");
    });
  const activeBtn = document.querySelector(
    `.metrics-sort-buttons button[onclick="sortMetrics('${field}')"]`
  );
  if (activeBtn) activeBtn.classList.add("active");
}
