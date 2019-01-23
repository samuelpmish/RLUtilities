import setuptools

with open('../../README.md', 'r') as fh:
    long_description = fh.read()

setuptools.setup(
    name='RLUtilities',
    version='0.0.1001',
    author='Sam Mish',
    author_email='samuelpmish@gmail.com',
    description='tools for controlling Rocket League bots',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/samuelpmish/rlutilities',
    packages=['rlutilities'],
    package_data={
        'rlutilities': [
            '**.*pyd'
        ]
    },
    python_requires='>=3',
    classifiers=[
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Operating System :: Microsoft :: Windows'
    ]
)
