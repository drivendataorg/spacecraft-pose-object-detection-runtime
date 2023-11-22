import importlib
import pytest

packages = [
    "pandas",
    "numpy",
]


@pytest.mark.parametrize("package", packages)
def test_package_can_be_imported(package):
    importlib.import_module(package)
