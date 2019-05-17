import setuptools

#with open('../README.md', 'r') as fh:
#    long_description = fh.read()

setuptools.setup(
    name='RLUtilities',
    version='1.0.2',
    author='Sam Mish',
    author_email='samuelpmish@gmail.com',
    description='tools for controlling Rocket League bots',
    #long_description=long_description,
    #long_description_content_type='text/markdown',
    url='https://github.com/samuelpmish/rlutilities',
    packages=['rlutilities'],
    package_data={
        'rlutilities': [
            '*.pyd',
            '__init__.py',
            '*.pyi',
        ]
    },
    python_requires='==3.7.*',
    classifiers=[
        'Programming Language :: Python :: 3.7',
        'Operating System :: Microsoft :: Windows'
    ]
)
