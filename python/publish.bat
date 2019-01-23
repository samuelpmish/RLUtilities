python -m pip install twine

python setup.py sdist bdist_wheel
python -m twine upload dist/*

@RD /S /Q "build"
@RD /S /Q "dist"
@RD /S /Q "rlutilities.egg-info"
