from fastapi import FastAPI, Request, UploadFile, File
from fastapi.responses import HTMLResponse, JSONResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
import shutil
import os

app = FastAPI()

app.mount("/static", StaticFiles(directory="assets"), name="static")
templates = Jinja2Templates(directory="templates")

dataInputDir = "../data/input/"

@app.get("/", response_class=HTMLResponse)
async def home(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})

@app.get("/config", response_class=HTMLResponse)
async def configPage(request: Request):
    return templates.TemplateResponse("config.html", {"request": request})

@app.get("/listFiles/")
async def listFiles():
    filesInfo = []
    os.makedirs(dataInputDir, exist_ok=True)
    for filename in os.listdir(dataInputDir):
        filePath = os.path.join(dataInputDir, filename)
        if filename.endswith(".txt"):
            with open(filePath, "r") as f:
                preview = "".join([next(f, "") for _ in range(2)]).strip()
            filesInfo.append({"name": filename, "preview": preview})
    return JSONResponse(content=filesInfo)

@app.post("/uploadFiles/")
async def uploadFiles(files: list[UploadFile] = File(...)):
    os.makedirs(dataInputDir, exist_ok=True)
    for file in files:
        filePath = os.path.join(dataInputDir, file.filename)
        with open(filePath, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)
    return {"status": "Archivos subidos exitosamente", "archivos": [file.filename for file in files]}
