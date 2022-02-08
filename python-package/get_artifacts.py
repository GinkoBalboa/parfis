import subprocess
import time
import requests
import json
import os

r = requests.get('https://api.github.com/repos/GinkoBalboa/parfis/actions/artifacts')
jr = json.loads(r.text)

currPath = os.path.dirname(os.path.abspath(os.path.expanduser(__file__)))

# Separate linux libs and windows libs for the four latest artifacts
win = []
linux = []
for i in range(4):
    if jr['artifacts'][i]['name'][-4:] == '.dll':
        win.append({'name': jr['artifacts'][i]['name'], 'url': jr['artifacts'][i]['url']})
    elif jr['artifacts'][i]['name'][-3:] == '.so':
        linux.append({'name': jr['artifacts'][i]['name'], 'url': jr['artifacts'][i]['url']})

# Get linux artifacts and create wheel

subprocess.Popen(f"del {currPath}\parfis\clib\*.so", shell=True)
time.sleep(3)

for i in range(2):
    r = requests.get(linux[i]['url'], allow_redirects=True)
    open(currPath + "/parfis/clib/" + linux[i]['name'], 'wb').write(r.content)

subprocess.Popen(f"del {currPath}\dist\*manylinux2014_x86_64*", shell=True)
time.sleep(3)

subprocess.Popen(f"python {currPath}/setup.py bdist_wheel --dist-dir={currPath}/dist --plat-name manylinux2014_x86_64", shell=True)
time.sleep(5)
