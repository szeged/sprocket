import os

# .../sprocket/tools
current_path = os.path.dirname(os.path.abspath(__file__))

chromium_folder_name = 'chromium'
sprocket_folder_name = 'sprocket'
chromium_path = os.path.join(current_path, '..', chromium_folder_name)
chromium_src_path = os.path.join(chromium_path, 'src')
sprocket_path = os.path.join(current_path, '..')
build_output_dir = os.path.join(chromium_src_path, 'out', 'Default')
