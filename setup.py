#****************************************************************************
#* setup.py for tblink-rpc-hdl
#****************************************************************************
import os
import subprocess
import sys
import shutil
import sysconfig
from setuptools import setup
from distutils.extension import Extension
from distutils.command.build_clib import build_clib
from distutils.ccompiler import new_compiler
from distutils.spawn import find_executable
from setuptools.command.build_ext import build_ext as _build_ext
from distutils.file_util import copy_file

_DEBUG = False
_DEBUG_LEVEL = 0

# First need to establish where things are
tblink_rpc_core_dir = os.path.dirname(os.path.abspath(__file__))

if os.path.isdir(os.path.join(tblink_rpc_core_dir, "packages")):
    print("Packages are inside this directory")
    packages_dir = os.path.join(tblink_rpc_core_dir, "packages")
else:
    parent = os.path.dirname(tblink_rpc_core_dir)
    
    if os.path.isdir(os.path.join(parent, "tblink-rpc-core")):
        print("tblink-rpc-core is a peer")
        packages_dir = parent
    else:
        raise Exception("Unexpected source layout")

# Now, build the core tblink-rpc-hdl library
cwd = os.getcwd()
if not os.path.isdir(os.path.join(cwd, "build")):
    os.makedirs(os.path.join(cwd, "build"))

if not os.path.isdir(os.path.join(tblink_rpc_core_dir, "python/tblink_rpc")):
    os.makedirs(os.path.join(tblink_rpc_core_dir, "python/tblink_rpc"))

env = os.environ.copy()
python_bindir = os.path.dirname(sys.executable)
print("python_bindir: %s" % str(python_bindir))

if "PATH" in env.keys():
    env["PATH"] = python_bindir + os.pathsep + env["PATH"]
else:
    env["PATH"] = python_bindir

# Run configure...
result = subprocess.run(
    ["cmake", 
     tblink_rpc_core_dir,
     "-GNinja",
     "-DCMAKE_BUILD_TYPE=Debug",
     "-DPACKAGES_DIR=%s" % packages_dir
     ],
    cwd=os.path.join(cwd, "build"),
    env=env)

if result.returncode != 0:
    raise Exception("cmake configure failed")

result = subprocess.run(
    ["ninja",
     "-j",
     "%d" % os.cpu_count()
     ],
    cwd=os.path.join(cwd, "build"),
    env=env)
if result.returncode != 0:
    raise Exception("build failed")

extra_compile_args = sysconfig.get_config_var('CFLAGS').split()
extra_compile_args = []
#extra_compile_args += ["-std=c++11", "-Wall", "-Wextra"]
if _DEBUG:
    extra_compile_args += ["-g", "-O0", "-DDEBUG=%s" % _DEBUG_LEVEL, "-UNDEBUG"]
else:
    extra_compile_args += ["-DNDEBUG", "-O3"]

def find_source(bases):
    ret = []
    for base in bases:
        for file in os.listdir(base):
            if os.path.splitext(file)[1] in (".cpp", ".c"):
                ret.append(os.path.join(base, file))
            
    print("find_source: " + str(ret))
    return ret

pybfms_root = os.path.dirname(os.path.abspath(__file__))

def _get_lib_ext_name():
    """ Get name of default library file extension on given OS. """

    if os.name == "nt":
        ext_name = "dll"
    else:
        ext_name = "so"

    return ext_name

class build_ext(_build_ext):
    def run(self):

        # TODO:
#        def_dir = os.path.join(cocotb_share_dir, "def")
#        self._gen_import_libs(def_dir)

        super().run()

    # Needed for Windows to not assume python module (generate interface in def file)
    def get_export_symbols(self, ext):
        return None

    # For proper cocotb library naming, based on https://github.com/cython/cython/issues/1740
#    def get_ext_filename(self, ext_name):
#        """
#        Like the base class method, but for libraries that are not python extension:
#         - removes the ``.cpython-36m-x86_64-linux-gnu.`` part before the extension
#         - replaces ``.pyd`` with ``.dll`` on windows.
#        """
#
#        filename = _build_ext.get_ext_filename(self, ext_name)
#
#        # for the simulator python extension library, leaving suffix in place
#        # TODO:
##        if "simulator" == os.path.split(ext_name)[-1]:
##            return filename
#
#        head, tail = os.path.split(filename)
#        tail_split = tail.split(".")
#
#        filename_short = os.path.join(head, tail_split[0] + "." + _get_lib_ext_name())
#
#        # icarus requires vpl extension, gpivpi is default in Makefiles
#        # TODO:
##        if "icarus" in filename:
##            filename_short = filename_short.replace("libvpi.so", "gpivpi.vpl")
##            filename_short = filename_short.replace("libvpi.dll", "gpivpi.vpl")
#
#        return filename_short

#    def finalize_options(self):
#        """ Like the base class method,but add extra library_dirs path. """
#
#        super().finalize_options()
#
#        for ext in self.extensions:
#            ext.library_dirs.append(
#                os.path.join(self.build_lib, os.path.dirname(ext._full_name))
#            )

    def copy_extensions_to_source(self):
        """ Like the base class method, but copy libs into proper directory in develop. """
        print("copy_extensions_to_source")
        super().copy_extensions_to_source()

        build_py = self.get_finalized_command("build_py")
        
        ext = self.extensions[0]
        fullname = self.get_ext_fullname(ext.name)
        filename = self.get_ext_filename(fullname)
        modpath = fullname.split(".")
        package = ".".join(modpath[:-1])
        package_dir = build_py.get_package_dir(package)

        copy_file(
            os.path.join(cwd, "build", "libtblink_rpc_core.so"),
            os.path.join(package_dir, "libtblink_rpc_core.so"))
                
        dest_filename = os.path.join(package_dir, filename)
        
        print("package_dir: %s dest_filename: %s" % (package_dir, dest_filename))
        
        return

        build_py = self.get_finalized_command("build_py")
        for ext in self.extensions:
            fullname = self.get_ext_fullname(ext.name)
            filename = self.get_ext_filename(fullname)
            modpath = fullname.split(".")
            package = ".".join(modpath[:-1])
            package_dir = build_py.get_package_dir(package)
            # unlike the method from `setuptools`, we do not call `os.path.basename` here
            dest_filename = os.path.join(package_dir, filename)
            src_filename = os.path.join(self.build_lib, filename)

            os.makedirs(os.path.dirname(dest_filename), exist_ok=True)

            copy_file(
                src_filename, dest_filename, verbose=self.verbose, dry_run=self.dry_run
            )
            if ext._needs_stub:
                self.write_stub(package_dir or os.curdir, ext, True)

    def _gen_import_libs(self, def_dir):
        """
        On Windows generate import libraries that contains the code required to
        load the DLL (.a) based on module definition files (.def)
        """

        # TODO:
        if os.name == "nt":
            for sim in ["icarus", "modelsim", "aldec"]:
                subprocess.run(
                    [
                        "dlltool",
                        "-d",
                        os.path.join(def_dir, sim + ".def"),
                        "-l",
                        os.path.join(def_dir, "lib" + sim + ".a"),
                    ]
                )

print("extra_compile_args=" + str(extra_compile_args))

setup(
  name = "tblink-rpc-core",
  packages=['tblink_rpc_core','tblink_rpc'],
  package_dir = {'' : 'python'},
  author = "Matthew Ballance",
  author_email = "matt.ballance@gmail.com",
  description = ("Provides the core TbLink-RPC library"),
  license = "Apache 2.0",
  keywords = ["SystemVerilog", "Verilog", "RTL", "Python"],
  url = "https://github.com/tblink-rpc/tblink-rpc-core",
  entry_points={
    'console_scripts': [
      'tblink-rpc = tblink_rpc.__main__:main'
    ]
  },
  install_requires=[
      'pyyaml'
  ],
  setup_requires=[
    'setuptools_scm',
    'cython'
  ],
  cmdclass={'build_ext': build_ext},
  ext_modules=[
        Extension("tblink_rpc_core.native",
            extra_compile_args=extra_compile_args,
#            include_dirs=[
#                os.path.join(pybfms_root, 'ext/common'), 
#                os.path.join(pybfms_root, 'ext/hdl_sim')],
            sources=[
                os.path.join(tblink_rpc_core_dir, 'python', "native.pyx"), 
                os.path.join(tblink_rpc_core_dir, 'python', 'invoke_req_closure.cpp'),
                os.path.join(tblink_rpc_core_dir, 'python', 'InterfaceImplClosure.cpp'),
                os.path.join(tblink_rpc_core_dir, 'python', 'TbLinkListenerClosure.cpp'),
                os.path.join(tblink_rpc_core_dir, 'python', 'TimeCallbackClosure.cpp'),
                os.path.join(tblink_rpc_core_dir, 'python', 'InterfaceImplFactoryProxy.cpp')
            ],
            language="c++",
            include_dirs=[
                os.path.join(tblink_rpc_core_dir, 'cpp'),
                os.path.join(tblink_rpc_core_dir, 'cpp', 'include')
            ]
#            libraries=[
#                "-Wl,--whole-archive",
#                "tblink_rpc_hdl",
#                "tblink_rpc_core",
#                "glog",
#                "-Wl,--no-whole-archive",
#            ]
        )
    ]
)


