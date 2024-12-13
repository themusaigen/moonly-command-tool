import os
import utility

def traverse_path_to_bundle(bundle, project: dict, path: str):
  # Получим директорию исходников.
  source_path = utility.get_source_path(project)
  
  # Получим init.lua
  init_path = os.path.join(source_path, "init.lua")
  
  for subdir, dirs, files in os.walk(path):
    # Если игнорируемая директория, тогда пропускаем.
    if utility.is_directory_ignored(project, subdir):
      continue
    
    for file in files:
      file_path = os.path.join(subdir, file)
      
      # Игнорируем корневой init.lua
      if file_path == init_path:
        continue
      
      with open(file_path, "r") as module:
        directories = subdir.split("\\")
        
        if file == "init.lua":
          package_name = ".".join(directories[1:])
        else:
          if len(directories) == 1:
            package_name = file.replace(".lua", "")
          else: 
            package_name = ".".join(directories[1:]) + f".{file.replace(".lua", "")}"
                    
        print(f" + Bundled {file_path} with package {package_name}")
          
        bundle.write(utility.generate_package(package_name, module.read(), file, file_path))