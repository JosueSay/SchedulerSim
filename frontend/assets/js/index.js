function initializeIndex() {
  console.log("[DEBUG] Entrando a '/', eliminando acceso previo.");
  denyAccess();

  ["procesos", "recursos", "acciones"].forEach((type) =>
    loadFilePreview(type, `${type}.txt`)
  );
}

function validateTxtContent(type, content) {
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

async function validateAndReadFile(file, type) {
  return new Promise((resolve) => {
    const reader = new FileReader();
    reader.onload = (e) => {
      const content = e.target.result;
      const isValid = validateTxtContent(type, content);
      resolve(isValid);
    };
    reader.readAsText(file);
  });
}

async function uploadAll() {
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
    validateAndReadFile(files.procesos, "procesos"),
    validateAndReadFile(files.recursos, "recursos"),
    validateAndReadFile(files.acciones, "acciones"),
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
    setTimeout(() => (window.location.href = "/config"), 1500);
  } else {
    showAlert("Error", "Error al subir los archivos.", "error");
  }
}

async function loadFilePreview(type, filename) {
  const response = await fetch("/listFiles/");
  const files = await response.json();
  const preview = files.find((f) => f.name === filename);
  const previewElement = document.getElementById(`preview-${type}`);
  if (previewElement) {
    previewElement.textContent = preview
      ? preview.preview
      : "Sin vista previa.";
  }
}
