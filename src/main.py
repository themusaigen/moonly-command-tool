import argparse
import json
import zipfile
import os

# Главный парсер
parser = argparse.ArgumentParser(prog='moonly')
subparsers = parser.add_subparsers(dest='command', title="Usage")

# Команда "init"
parser_init = subparsers.add_parser('init', help='Initializes new moonly project')

# Команда "build"
parser_build = subparsers.add_parser("build", help="Builds moonloader archive from moonly project")

# Парсим
args = parser.parse_args()

if args.command == "init":
  content = {
    "name": "my-project",
    "library": "lib",
    "source": "src"
  }
  
  with open("project.json", "w") as project:
    project.write(json.dumps(content, indent=2))
    
  print("initialized default project")
elif args.command == "build":
  with open("project.json", "r") as project:
    # Читаем содержимое project.json
    content = json.load(project)

    # На случай, если не указано имя проекта в поле name.
    folders = os.getcwd().split("\\")
  
    # Получим исходники и имя проекта. 
    try:
      name = content["name"]
    except KeyError:
      # На случай, если не указано имя проекта в поле name.
      folders = os.getcwd().split("\\")

      name = folders[len(folders) - 1]   

    try:
      source_path = content["source"]
    except KeyError:
      source_path = "src"
    
    try:
      library_path = content["library"]
    except KeyError:
      library_path = "lib"
    
    # Получим путь к init.lua
    init_path = os.path.join(source_path, "init.lua")
    
    # Путь к библиотекам.

    # Создаём архив.
    with zipfile.ZipFile(name + ".zip", mode="w") as zip:
      # Записываем init файл.
      zip.write(init_path, name + ".lua")
      
      print("= Processing source code.")
      print(f"  + Add init file: {init_path}")
      
      # Проходимся по всему содержимому src
      for subdir, dirs, files in os.walk(source_path):
        for file in files:
            path = os.path.join(subdir, file)
            
            # Не трогаем src/init.lua
            if path == init_path:
              continue
      
            # Записываем файл.
            zip.write(path, path.replace(source_path + "\\", ""))
            
            # Лог для юзера            
            print(f"  + Add source file: {path}")
      
      print("= Add libraries.")
      
      # Проходимся по библиотекам.
      for subdir, dirs, files in os.walk(library_path):
        for file in files:
            path = os.path.join(subdir, file)
      
            # Записываем библиотеку.
            zip.write(path)
            
            # Лог для юзера
            print(f"  + Add library file: {path}")
