CYTHON_SRC=$(wildcard stitchup/lib/*.pyx)


clean-pyc:
	find . -name '*.pyc' -exec rm --force {} +
	find . -name '*.pyo' -exec rm --force {} +
	find . -name '*~' -exec rm --force  {} +
	find . -name __pycache__ -exec rm -rf  {} +

clean: clean-pyc
	rm -rf build dist
	rm -rf *.egg-info

cython-inplace: clean-cython
	python setup.py build_ext --inplace

clean-cython:
	rm -rf $(patsubst %.pyx,%.cpp,$(CYTHON_SRC))
	rm -rf $(patsubst %.pyx,%.so,$(CYTHON_SRC))
