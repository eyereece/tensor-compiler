import lit.formats
import os

config.name = 'MyDLC'
config.excludes = ['lit.cfg.py']
config.suffixes = ['.mlir', '.py']

config.test_format = lit.formats.ShTest(execute_external=True)

config.test_source_root = os.path.dirname(__file__)

config.substitutions.append(('%driver', os.path.join(os.getcwd(), 'driver')))

llvm_bin_dir = os.path.abspath("../../../llvm-project/build/bin")

config.environment['PATH'] = os.path.pathsep.join([llvm_bin_dir, config.environment.get('PATH', '')])