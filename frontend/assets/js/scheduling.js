function initializeScheduling() {
  console.log(
    "[DEBUG] Entrando a /upload-scheduling, eliminando acceso previo."
  );
  denyAccess();
  loadFilePreview("procesos", "procesos.txt");
}

function validateTxtContent(content) {
  const lines = content.trim().split("\n");
  const pattern = /^[A-Za-z0-9]+,\s*\d+,\s*\d+,\s*\d+$/;

  for (const [index, line] of lines.entries()) {
    if (!pattern.test(line.trim())) {
      showAlert(
        "Error de Formato",
        `Archivo: <strong>procesos.txt</strong><br>Línea ${
          index + 1
        }:<br>"${line.trim()}"<br>Corrige el formato.`,
        "error"
      );
      return false;
    }
  }
  return true;
}

async function uploadScheduling() {
  const file = document.getElementById("procesos").files[0];

  if (!file) {
    showAlert(
      "Archivo Faltante",
      "Debes seleccionar el archivo procesos.txt.",
      "warning"
    );
    return;
  }

  if (
    file.name.toLowerCase() !== "procesos.txt" ||
    !file.name.toLowerCase().endsWith(".txt")
  ) {
    showAlert(
      "Nombre o formato incorrecto",
      "El archivo debe llamarse <strong>procesos.txt</strong> y tener extensión .txt",
      "warning"
    );
    return;
  }

  const content = await file.text();
  if (!validateTxtContent(content)) return;

  const formData = new FormData();
  formData.append("files", file);

  const response = await fetch("/uploadFiles/", {
    method: "POST",
    body: formData,
  });

  if (response.ok) {
    showAlert("Éxito", "Archivo subido correctamente.", "success");
    allowAccess();
    setTimeout(() => (window.location.href = "/config"), 1500);
  } else {
    showAlert("Error", "Error al subir el archivo.", "error");
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
