/**
 * Inicializa la sección de carga para planificación (scheduling).
 * Elimina el acceso previo mediante la función `denyAccess()`.
 * No recibe parámetros ni retorna valor.
 */
function initializeScheduling() {
  console.log(
    "[DEBUG] Entrando a /upload-scheduling, eliminando acceso previo."
  );
  denyAccess();
}

/**
 * Valida el contenido de un archivo `procesos.txt` asegurando que
 * cada línea siga el patrón esperado: ID, tiempo de llegada, duración, prioridad.
 *
 * @param {string} content - Contenido del archivo como texto plano.
 * @return {boolean} - `true` si todas las líneas cumplen con el formato, `false` en caso contrario.
 */
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

/**
 * Realiza la carga del archivo `procesos.txt` para la planificación.
 * Verifica existencia, nombre y formato del archivo, valida su contenido y lo sube vía `fetch`.
 * Muestra alertas en caso de error y redirecciona si la carga es exitosa.
 *
 * @return {Promise<void>} - No retorna un valor, pero ejecuta acciones asíncronas.
 */
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
      "El archivo debe llamarse procesos.txt y tener extensión .txt",
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
    setTimeout(() => (window.location.href = "/config-scheduling"), 1500);
  } else {
    showAlert("Error", "Error al subir el archivo.", "error");
  }
}
