import os
import sys
import platform
import subprocess
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        super().__init__(name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        # 检查 CMake 是否安装
        try:
            subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("必须安装 CMake 并确保它可用")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name))
        )
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            "-DCMAKE_BUILD_TYPE=Release",
        ]

        # 如果是 Windows 且使用 MinGW，则添加特定参数
        if platform.system() == "Windows":
            cmake_args += [
                "-G", "MinGW Makefiles",
                "-DCMAKE_SH=\"CMAKE_SH-NOTFOUND\"",
                "-DCMAKE_C_COMPILER=gcc.exe",
                "-DCMAKE_CXX_COMPILER=g++.exe",
            ]

        build_temp = self.build_temp
        if not os.path.exists(build_temp):
            os.makedirs(build_temp)

        # 执行 CMake 配置
        subprocess.check_call(
            ['cmake', ext.sourcedir] + cmake_args,
            cwd=build_temp
        )
        # 执行编译
        subprocess.check_call(
            ['cmake', '--build', '.', '--config', 'Release'],
            cwd=build_temp
        )


setup(
    name='jsp',
    version='1.0',
    author='Ming Qi',
    author_email='qiming01@outlook.com',
    description='Job Shop Scheduling Python Module',
    ext_modules=[CMakeExtension('jsp', sourcedir='../../')],
    cmdclass={'build_ext': CMakeBuild},
    zip_safe=False,
)
