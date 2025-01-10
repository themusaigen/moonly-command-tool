import argparse
import json
import zipfile
import os
import utility
import bundler

# Главный парсер.
parser = argparse.ArgumentParser(prog='moonly')
subparsers = parser.add_subparsers(dest='command', title="Usage")

# Команда "init"
parser_init = subparsers.add_parser('init', help='Initializes new moonly project')
parser_init.add_argument('-n', '--name', default="my-project", help="Specify name of the new project.")
parser_init.add_argument('-s', '--sourcedir', default='src', help="Specify the source directory of the new project.")
parser_init.add_argument('-l', '--libdir', default='lib', help="Specfiy the library directory of the new project.")
parser_init.add_argument('-b', '--builddir', default="dist", help="Specify the directory used for build command output.")
parser_init.add_argument('-d', '--distdir', default="dist", help="Specify the directory used for bundle command output.")
parser_init.add_argument('-vs', '--vscode', action="store_true", help="Automatically create .vscode/settings.json.")
parser_init.add_argument("-dirs", "--create-dirs", action="store_true", help="Automatically create main directories.")

# Команда "build"
parser_build = subparsers.add_parser("build", help="Builds moonloader archive from moonly project")

# Команда "bundle"
parser_bundle = subparsers.add_parser("bundle", help="Bundles all moonly project into one Lua file for distribution")

# Парсим
args = parser.parse_args()

if args.command == "init":
  args = parser.parse_args()
  
  print(args)
  
  content = {
    "name": args.name,
    "library": args.libdir,
    "source": args.sourcedir,
    "build": {
      "output": args.builddir,
      "additionalDirs": []
    },
    "distribute": {
      "output": args.distdir,
      "additionalDirs": [],
      "ignoredDirs": [],
    }
  }
  
  with open("project.json", "w") as project:
    project.write(json.dumps(content, indent=2))
  
  if args.vscode:
    # Создадим папку .vscode
    try:
      os.mkdir(".vscode")
    except:
      pass
    
    sumneko = {
      "[lua]": {
        "editor.defaultFormatter": "sumneko.lua",
        "files.encoding": "windows1251"
      },
      "Lua.runtime.version": "LuaJIT",
      "Lua.runtime.path": [
        f"{args.sourcedir}/?.lua",
        f"{args.sourcedir}/?/init.lua",
        f"{args.libdir}/?.lua",
        f"{args.libdir}/?/init.lua"
      ],
      "Lua.diagnostics.globals": [
        "main"
      ]
    }
    
    # Создадим файл с настройками и запушим default настройки.
    with open(".vscode/settings.json", "w") as settings:
      settings.write(json.dumps(sumneko, indent=2))
  
  # Авто-создание основных директорий.
  if args.create_dirs:
    try:
      os.mkdir(args.sourcedir)
    except:
      pass
    
    try:
      os.mkdir(args.libdir)
    except:
      pass
  
  print("= Initialized project.")
  print(f"  = Name: {args.name}")
  print(f"  = Source directory: {args.sourcedir}")
  print(f"  = Libraries directory: {args.libdir}")
  print(f"  = Build directory: {args.builddir}")
  print(f"  = Distribute directory: {args.distdir}")
  print(f"  = Visual Studio Code settings: {args.vscode and "Yes" or "No"}")
  print(f"  = Auto-create main directories: {args.create_dirs and "Yes" or "No"}")
elif args.command == "build":
  with open("project.json", "r") as project:
    # Читаем содержимое project.json
    content = json.load(project)

    # Получим исходники и имя проекта. 
    name = utility.get_project_name(content)
    source_path = utility.get_source_path(content)
    library_path = utility.get_libraries_path(content)
    
    # Получим путь к init.lua
    init_path = os.path.join(source_path, "init.lua")

    # Создаём директорию.    
    try:
      os.mkdir(utility.get_build_path(content))
    except:
      pass # Директория уже создана, пропускаем.

    # Создаём архив.
    with zipfile.ZipFile(f"{utility.get_build_path(content)}/{name}.zip", mode="w") as zip:
      # Записываем init файл.
      zip.write(init_path, name + "-init.lua")
      
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
      
      print("= Processing libraries.")
      
      # Проходимся по библиотекам.
      for subdir, dirs, files in os.walk(library_path):
        for file in files:
            path = os.path.join(subdir, file)
      
            # Записываем библиотеку.
            zip.write(path)
            
            # Лог для юзера
            print(f"  + Add library file: {path}")
      
      print("= Processing additional directories.")
      
      # Проходимся по дополнительным директориям.
      for dir in utility.get_build_additional_dirs(content):
        for subdir, dirs, files in os.walk(dir):
          for file in files:
            path = os.path.join(subdir, file)
            
            # Записываем доп. файл.
            zip.write(path)
            
            # Лог для юзера.
            print(f"  + Add additional file: {path}")
            
elif args.command == "bundle":
  with open("project.json", "r") as project:
    # Читаем содержимое project.json
    content = json.load(project)
    
    # Создаём директорию.    
    try:
      os.mkdir(utility.get_distribute_path(content))
    except:
      pass # Директория уже создана, пропускаем.
    
    # Получим директорию исходников.
    source_path = utility.get_source_path(content)
    
    # Получим init.lua
    init_path = os.path.join(source_path, "init.lua")
    
    # Создадим корневой файл.
    with open(utility.get_distribute_core_path(content), "w") as core:
      # Напишем копирайты (хехе).
      core.write("-- Bundled using <moonly>\n")
      core.write("-- Get moonly at <https://github.com/themusaigen/moonly>\n\n")
      
      # Проходимся по сурсам.
      bundler.traverse_path_to_bundle(core, content, source_path)
      
      # Проходимся по доп. папкам.
      for path in utility.get_additional_dirs(content):
        bundler.traverse_path_to_bundle(core, content, utility.convert_backslashes(path))
      
      # Добавляем корневой файл.
      with open(init_path, "r") as init_file:
        core.write("-- Core file <init.lua>\n")
        
        core.write(init_file.read())
        
        print(" + Bundled init.lua")
            
        