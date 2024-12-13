import os

# Утилита для получения имени проекта.
def get_project_name(content: dict) -> str:
  try:
    return content["name"]
  except KeyError:
    # На случай, если не указано имя проекта в поле name.
    folders = os.getcwd().split("\\")

    return folders[len(folders) - 1]  

# Утилита для получения пути к исходникам.
def get_source_path(content: dict) -> str:
  try:
    return content["source"]
  except KeyError:
    return "src"

# Утилита для получения пути к библиотекам.
def get_libraries_path(content: dict) -> str:
  try:
    return content["library"]
  except KeyError:
    return "lib"
  
# Утилита для получения пути к доп. каталогам при сборке.
def get_build_additional_dirs(content: dict) -> list[dict]:
  try:
    return content["build"]["additionalDirs"]
  except KeyError:
    return []
  
# Утилита для получения выходной директории при сборке.
def get_build_path(content: dict) -> str:
  try:
    return content["build"]["output"]
  except KeyError:
    return "dist"
  
# Утилита для получения пути к директории для бандлинга.
def get_distribute_path(content: dict) -> str:
  try:
    return content["distribute"]["output"]
  except KeyError:
    return "dist"
  
# Утилита для получения пути к bundle-файлу.
def get_distribute_core_path(content: dict) -> str:
  try:
    return f"{content["distribute"]["output"]}/{content["name"]}.lua"
  except KeyError:
    return f"dist/{get_project_name(content)}.lua" 

# Утилита для получения списка игнорируемых директорий.
def get_distribute_ignored_directories(content: dict) -> list[str]:
  try:
    return content["distribute"]["ignoredDirs"]
  except KeyError:
    return []    

# Утилита для конвертации бэкслешей
def convert_backslashes(input: str) -> str:
  return input.replace("/", "\\")

# Утилита для проверки, является ли директория игнориуемой.
def is_directory_ignored(content: dict, dir: str) -> bool:
  for ignored in get_distribute_ignored_directories(content):
    if convert_backslashes(ignored) in dir:
      return True
  
  return False

# Утилита для получения списка допольнителных директорий для бандлинга. 
def get_additional_dirs(content: dict) -> list[str]:
  try:
    return content["distribute"]["additionalDirs"]
  except KeyError:
    return []  
    

# Генератор package.preload
def generate_package(package_name: str, data: str, filename: str, path: str) -> str:  
  return f"""-- Module <{filename}> ({path})
package.preload["{package_name}"] = function(...)\n{data}\nend\n
"""
    