from _jep import forName
import sys
from types import ModuleType

def jep_trace(str):
    #print(str)
    pass

class module(ModuleType):
    """Lazy load classes not found at runtime.

    Introspecting Java packages is difficult, there is not a good
    way to get a list of all classes for a package. By providing
    a __getattr__ implementation for modules, this class can
    try to find classes manually.

    Based on the ClassEnquirer used, some classes may not appear in dir()
    but will import correctly.
    """

    def __getattr__(self, name):
        jep_trace("module.__getattr__ " + self.__name__ + " . " + name)
        jep_trace("module.__getattr__ dict="+str(self.__dict__))
        try:
            if self.__java_module__:
#                jep_trace("module.__getattr__ __getattr__ for java module methods" + str(self.__java_module__.__methods__))
#                jep_trace("module.__getattr__ __getattr__ for java module members" + str(self.__java_module__.__members__))
                m = getattr(self.__java_module__, name)
                jep_trace("module.__getattr__ getAttribute java attr is " + str(m))
                if not m:
                    raise AttributeError()
                return m
            else:
                jep_trace("module.__getattr__ calling super")
                s = super(module, self)
                jep_trace("module.__getattr__ s = " + str(s) + "::::" + str(s.__dict__))
                m = s.__getattribute__(name)
                jep_trace("module.__getattr__ m = " + str(m))
                return m
        except AttributeError as ae:
            jep_trace("module.__getattr__ AttributeError="+str(ae))
            fullname = self.__name__ + '.' + name
            if (hasattr(self.__classEnquirer__, "isJavaModule") and self.__classEnquirer__.isJavaModule(fullname)):
                jep_trace("module.__getattr__ creating java module for " + fullname)
                mod = self.__classEnquirer__.loadJavaModule(fullname)
                jep_trace("module.__getattr__ created java module")
                if fullname in sys.modules:
                    mod = sys.modules[fullname]
                    return patchJavaModule(mod, fullname, self.__loader__, self.__classEnquirer__)
            else:
                jep_trace("module.__getattr__ calling getSubPackages")
                subpkgs = self.__classEnquirer__.getSubPackages(self.__name__)
                jep_trace("module.__getattr__ called getSubPackages subpkgs = " + str(subpkgs))
                if subpkgs and name in subpkgs:
                    jep_trace("module.__getattr__ found subPackage " + name) 
                    mod = makeModule(fullname, self.__loader__, self.__classEnquirer__)
                    jep_trace("module.__getattr__ in subpkg mod = " + str(mod))
                    return mod
                elif name == '__all__':
                    s = self.__dir__()
                    jep_trace("module.__getattr__ s = " + str(s))
                    return s
                else:
                    jep_trace("module.__getattr__ final else") 
                    if self.__java_module__:
                        jep_trace("module.__getattr__ final else is __java_module__") 
                        return self.__java_module__
                    else:
                        jep_trace("module.__getattr__ else")
                        # assume it is a class and attempt the import
                        clazz = forName('{0}.{1}'.format(self.__name__, name))
                        jep_trace("module.__getattr__ clazz = " + str(clazz))
                        setattr(self, name, clazz)
                        return clazz
    def __call__(self, *args):
        jep_trace("module.__call__ called")
        return super().__call__(args)

    def __dir__(self):
        jep_trace("module.__dir__ for " + self.__name__)
        result = []
        subpkgs = self.__classEnquirer__.getSubPackages(self.__name__)
        if subpkgs:
            for s in subpkgs:
                result.append(s)
        classnames = self.__classEnquirer__.getClassNames(self.__name__)
        if classnames:
            for c in classnames:
                result.append(c.split('.')[-1])
        jep_trace("module.__all__ result = " + str(result))
        return result


def makeModule(fullname, loader, classEnquirer):
    mod = module(fullname)
    mod.__dict__.update({
        '__loader__': loader,
        '__path__': [],
        '__file__': '<java>',
        '__package__': None,
        '__classEnquirer__': classEnquirer,
        '__java_module__':None
        })
    sys.modules[fullname] = mod
    return mod

def makeJavaModule(fullname, jmodule):
    mod = module(fullname)
    mod.__dict__.update({
        '__loader__': None,
        '__path__': [],
        '__file__': '<java>',
        '__package__': None,
        '__classEnquirer__': None,
        '__java_module__':jmodule,
        })
    sys.modules[fullname] = mod
    return mod

def patchJavaModule(mod, fullname, loader, classEnquirer):
    mod.__dict__.update({
    '__loader__': loader,
    '__classEnquirer__': classEnquirer
    })
    sys.modules[fullname] = mod
    return mod

class JepJavaImporter(object):
    def __init__(self, classEnquirer=None):
        if classEnquirer:
            self.classEnquirer = classEnquirer
        else:
            self.classEnquirer = forName('jep.ClassList').getInstance()

    def find_module(self, fullname, path=None):
        jep_trace("find_module " + fullname + " in " + str(sys.path))
        if self.classEnquirer.isJavaPackage(fullname):
            return self  # found a Java package with this name
        elif hasattr(self.classEnquirer, "isJavaModule") and self.classEnquirer.isJavaModule(fullname):
            return self
        return None

    def load_module(self, fullname):
        jep_trace("load_module " + fullname)
        if fullname in sys.modules:
            jep_trace("load_module " + fullname + " found is sys.modules")
            return sys.modules[fullname]
        if hasattr(self.classEnquirer, "isJavaModule") and self.classEnquirer.isJavaModule(fullname):
            mod = self.classEnquirer.loadJavaModule(fullname)
            if fullname in sys.modules:
                mod = sys.modules[fullname]
                mod = patchJavaModule(mod, fullname, self, self.classEnquirer)
        else:
            mod = makeModule(fullname, self, self.classEnquirer)
        jep_trace("load_module mod = " + str(mod))
        return mod


def setupImporter(classEnquirer):
    alreadySetup = False
    for importer in sys.meta_path:
        if isinstance(importer, JepJavaImporter):
            alreadySetup = True
            break
    if not alreadySetup:
        sys.meta_path.insert(0,JepJavaImporter(classEnquirer))

def registerJavaModule(fullname, jmodule):
    jep_trace("registerJavaModule " + fullname + " = "  + str(jmodule))
    makeJavaModule(fullname, jmodule)

def unregisterJavaModule(fullname):
    jep_trace("unregisterJavaModule " + fullname)
    del sys.modules[fullname]

def teardownImporter():
    for key in list(sys.modules.keys()):
        module = sys.modules[key]
        if "__java_module__" in module.__dict__:
            if module.__java_module__:
                del sys.modules[key]
