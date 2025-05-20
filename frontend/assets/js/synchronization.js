/**
 * Inicializa el proceso de carga de sincronización denegando el acceso previo.
 * Se muestra un mensaje en consola y se llama a `denyAccess()` para prevenir acceso hasta completar validación.
 *
 * @return {void}
 */
function initializeSyncUpload() {
  console.log(
    "[DEBUG] Entrando a /upload-synchronization, eliminando acceso previo."
  );
  denyAccess();
}

/**
 * Valida el contenido de un archivo `.txt` según su tipo (procesos, recursos o acciones).
 * Verifica que cada línea cumpla un patrón específico predefinido por tipo.
 * Muestra una alerta si alguna línea tiene un formato inválido.
 *
 * @param {string} type - Tipo del archivo: "procesos", "recursos" o "acciones".
 * @param {string} content - Contenido completo del archivo a validar.
 * @return {boolean} `true` si el contenido es válido; de lo contrario, `false`.
 */
function validateSyncTxtContent(type, content) {
  const lines = content.trim().split("\n");
  const regexPatterns = {
    procesos: /^[A-Za-z0-9]+,\s*\d+,\s*\d+,\s*\d+$/,
    recursos: /^[A-Za-z0-9]+,\s*\d+$/,
    acciones: /^[A-Za-z0-9]+,\s*(READ|WRITE),\s*[A-Za-z0-9]+,\s*\d+$/,
  };

  const pattern = regexPatterns[type];
  if (!pattern) return false;

  for (const [index, line] of lines.entries()) {
    if (!pattern.test(line.trim())) {
      showAlert(
        "Error de Formato",
        `Archivo: <strong>${type}.txt</strong><br>Línea ${
          index + 1
        }:<br>"${line.trim()}"<br>Corrige el formato.`,
        "error"
      );
      return false;
    }
  }
  return true;
}

/**
 * Lee y valida un archivo `.txt` de forma asíncrona.
 * Usa FileReader para leer el archivo como texto y valida su contenido.
 *
 * @param {File} file - Objeto `File` del input HTML.
 * @param {string} type - Tipo del archivo: "procesos", "recursos" o "acciones".
 * @return {Promise<boolean>} Promesa que resuelve en `true` si el contenido es válido; de lo contrario, `false`.
 */
async function validateAndReadSyncFile(file, type) {
  return new Promise((resolve) => {
    const reader = new FileReader();
    reader.onload = (e) => {
      const content = e.target.result;
      const isValid = validateSyncTxtContent(type, content);
      resolve(isValid);
    };
    reader.readAsText(file);
  });
}

/**
 * Valida la selección, nombre y formato de los archivos requeridos (procesos.txt, recursos.txt, acciones.txt).
 * Verifica que el contenido de cada archivo sea válido.
 * Si todo es correcto, sube los archivos al backend y muestra alertas según el resultado.
 *
 * @return {Promise<void>} No retorna valor, pero gestiona todo el proceso de validación y subida.
 */
async function uploadAllSyncFiles() {
  const files = {
    procesos: document.getElementById("procesos").files[0],
    recursos: document.getElementById("recursos").files[0],
    acciones: document.getElementById("acciones").files[0],
  };

  if (!files.procesos || !files.recursos || !files.acciones) {
    showAlert(
      "Archivos Incompletos",
      "Debes seleccionar los 3 archivos antes de subir.",
      "warning"
    );
    return;
  }

  if (
    files.procesos.name.toLowerCase() !== "procesos.txt" ||
    files.recursos.name.toLowerCase() !== "recursos.txt" ||
    files.acciones.name.toLowerCase() !== "acciones.txt"
  ) {
    showAlert(
      "Nombre Incorrecto",
      "Los archivos deben llamarse exactamente:<br>- procesos.txt<br>- recursos.txt<br>- acciones.txt",
      "warning"
    );
    return;
  }

  if (
    !files.procesos.name.toLowerCase().endsWith(".txt") ||
    !files.recursos.name.toLowerCase().endsWith(".txt") ||
    !files.acciones.name.toLowerCase().endsWith(".txt")
  ) {
    showAlert(
      "Formato Incorrecto",
      "Todos los archivos deben tener extensión <strong>.txt</strong>.",
      "warning"
    );
    return;
  }

  const validations = await Promise.all([
    validateAndReadSyncFile(files.procesos, "procesos"),
    validateAndReadSyncFile(files.recursos, "recursos"),
    validateAndReadSyncFile(files.acciones, "acciones"),
  ]);

  if (validations.includes(false)) return;

  const formData = new FormData();
  formData.append("files", files.procesos);
  formData.append("files", files.recursos);
  formData.append("files", files.acciones);

  const response = await fetch("/uploadFiles/", {
    method: "POST",
    body: formData,
  });

  if (response.ok) {
    showAlert("Éxito", "Archivos subidos correctamente.", "success");
    allowAccess();
    setTimeout(() => (window.location.href = "/config-synchronization"), 1500);
  } else {
    showAlert("Error", "Error al subir los archivos.", "error");
  }
}
