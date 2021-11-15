import sys
import os
import subprocess
import pkg_resources

def install(packages: list):
    print(f"Installing python modules %s ..." % (', '.join(f"'{pkg}'" for pkg in packages)))
    subprocess.check_call([sys.executable, '-m', 'pip', 'install'] + packages)

# from github.com/TheCherno/Hazel
def _ValidatePackages(packages):
    required = set(packages)
    installed = { pkg.key for pkg in pkg_resources.working_set }
    missing = required - installed
    if (missing):
        install(list(missing))

def ValidatePackages():
    packages = ['requests']
    _ValidatePackages(packages)

def ValidateWorkingDir():
    if os.path.exists('SetupVulkan.py'):
        print('Executed from \'scripts\' directory. Changing to root directory')
        os.chdir('../')
    elif os.path.isdir('scripts'):
        pass
    else:
        print('Please execute the script from the project root directory!')
        quit()
