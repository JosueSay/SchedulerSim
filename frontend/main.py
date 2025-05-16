from fastapi import FastAPI, Request, UploadFile, File
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import shutil
import os

app = FastAPI()
app.mount("/static", StaticFiles(directory="assets"), name="static")
templates = Jinja2Templates(directory="templates")

DATA_INPUT_DIR = "../data/input/"

@app.get("/", response_class=HTMLResponse)
async def home(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})

@app.get("/files/")
async def list_files():
    files_info = []
    for filename in os.listdir(DATA_INPUT_DIR):
        file_path = os.path.join(DATA_INPUT_DIR, filename)
        if filename.endswith(".txt"):
            with open(file_path, "r") as f:
                preview = "".join([next(f, "") for _ in range(2)]).strip()
            files_info.append({"name": filename, "preview": preview})
    return files_info


@app.post("/upload/")
async def upload_files(files: list[UploadFile] = File(...)):
    os.makedirs(DATA_INPUT_DIR, exist_ok=True)
    for file in files:
        file_path = os.path.join(DATA_INPUT_DIR, file.filename)
        with open(file_path, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)
    return {"status": "Archivos subidos exitosamente", "archivos": [file.filename for file in files]}


@app.get("/config", response_class=HTMLResponse)
async def config_page(request: Request):
    return templates.TemplateResponse("config.html", {"request": request})
