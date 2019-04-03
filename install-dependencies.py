#!/usr/bin/env python3
import os
import urllib.request
import zipfile

external_dir = "external"
os.makedirs(external_dir, exist_ok=True)
os.chdir(external_dir)

zip_file = 'master.zip'
urllib.request.urlretrieve('https://github.com/g-truc/gli/archive/master.zip', zip_file)

zip_ref = zipfile.ZipFile(zip_file, 'r')
zip_ref.extractall('.')
zip_ref.close()

os.rename('gli-master', 'gli')

os.makedirs('fx', exist_ok=True)
os.chdir('fx')
urllib.request.urlretrieve("https://raw.githubusercontent.com/jessey-git/fx-gltf/master/include/fx/gltf.h", "gltf.h")
os.chdir('..')

os.makedirs('imgui', exist_ok=True)
os.chdir('imgui')
urllib.request.urlretrieve("https://raw.githubusercontent.com/ocornut/imgui/v1.66/examples/imgui_impl_opengl3.h", "imgui_impl_opengl3.h")
urllib.request.urlretrieve("https://raw.githubusercontent.com/ocornut/imgui/v1.66/examples/imgui_impl_opengl3.cpp", "imgui_impl_opengl3.cpp")
urllib.request.urlretrieve("https://raw.githubusercontent.com/ocornut/imgui/v1.66/examples/imgui_impl_sdl.h", "imgui_impl_sdl.h")
urllib.request.urlretrieve("https://raw.githubusercontent.com/ocornut/imgui/v1.66/examples/imgui_impl_sdl.cpp", "imgui_impl_sdl.cpp")
os.chdir('..')

os.chdir('..')
