import setuptools
from distutils.core import setup, Extension
import platform

LONGDOC = 'Fast C Short Path chinese and english cut'


if platform.python_version().startswith('2'):
    otter_py2 = Extension('_otter_funcs_py2',
        include_dirs=['.'],
        sources=['_otter_funcs_py2.c'],
        library_dirs=['.'],
        libraries=['otter','enchant','stdc++','glib-2.0','gmodule-2.0'],
    )
    setup(name='otter',
          version='0.10',
          description='Use C  Chinese Words Segementation Utilities',
          long_description=LONGDOC,
          author='xuen',
          author_email='nanhangxuen@163.com',
          url='https://github.com/xuenhappy/otter',
          license="Apache2",
          classifiers=[
            'Intended Audience :: Developers',
            'License :: OSI Approved :: Apache2 License',
            'Operating System :: OS Independent',
            'Natural Language :: Chinese (Simplified)',
            'Natural Language :: Chinese (Traditional)',
            'Programming Language :: Python',
            'Programming Language :: Python :: 2',
            'Programming Language :: Python :: 2.6',
            'Programming Language :: Python :: 2.7',
            'Programming Language :: Python :: 3.4',
            'Programming Language :: Python :: 3.5',
            'Programming Language :: Python :: 3.7',
            'Topic :: Text Processing',
            'Topic :: Text Processing :: Indexing',
            'Topic :: Text Processing :: Linguistic',
        ],
        keywords='NLP,tokenizing,Chinese word segementation',
        packages=['otter'],
        package_dir={'otter':'otter'},
        package_data={'otter':['otter/*.py','dict/*.*']},
        ext_modules = [otter_py2],
    )



import os
import sysconfig
def get_ext_filename_without_platform_suffix(filename):
    name, ext = os.path.splitext(filename)
    ext_suffix = sysconfig.get_config_var('EXT_SUFFIX')

    if ext_suffix == ext:
        return filename

    ext_suffix = ext_suffix.replace(ext, '')
    idx = name.find(ext_suffix)

    if idx == -1:
        return filename
    else:
        return name[:idx] + ext





if platform.python_version().startswith('3'):
    from Cython.Build import cythonize
    from Cython.Distutils import build_ext
    class BuildExtWithoutPlatformSuffix(build_ext):
        def get_ext_filename(self, ext_name):
            filename = super().get_ext_filename(ext_name)
            return get_ext_filename_without_platform_suffix(filename)

    otter_py3 = Extension('_otter_funcs_py3',
            include_dirs=['.'],
            library_dirs=['.'],
            sources=['_otter_funcs_py3.c'],
            libraries=['otter','enchant','stdc++','glib-2.0','gmodule-2.0'],
    )
    setup(name='otter',
        cmdclass={'build_ext': BuildExtWithoutPlatformSuffix},
        version='0.10',
        description='Use C  Chinese Words Segementation Utilities',
        long_description=LONGDOC,
        author='xuen',
        author_email='nanhangxuen@163.com',
        url='https://github.com/xuenhappy/otter',
        license="Apache2",
        classifiers=[
            'Intended Audience :: Developers',
            'License :: OSI Approved :: Apache2 License',
            'Operating System :: OS Independent',
            'Natural Language :: Chinese (Simplified)',
            'Natural Language :: Chinese (Traditional)',
            'Programming Language :: Python',
            'Programming Language :: Python :: 2',
            'Programming Language :: Python :: 2.6',
            'Programming Language :: Python :: 2.7',
            'Programming Language :: Python :: 3.4',
            'Programming Language :: Python :: 3.5',
            'Programming Language :: Python :: 3.7',
            'Topic :: Text Processing',
            'Topic :: Text Processing :: Indexing',
            'Topic :: Text Processing :: Linguistic',
        ],
        keywords='NLP,tokenizing,Chinese word segementation',
        packages=['otter','otter'],
        package_dir={'otter':'otter'},
        package_data={'otter':['otter/*.py','dict/*.*']},
        ext_modules = [otter_py3],
)
