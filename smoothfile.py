from smoothsource import smoothsource

import os
import math
import matplotlib.pyplot as plt
import numpy as np

def _default(*args):
    return True

def _plot(parameter=None):
    times = []
    values = []
    positionxs = []
    positionys = []
    positionzs = []
    directions = []
    with open("player.ofs", 'r') as f:
        data = f.read()
        for line in data.splitlines():
            columns = line.split(" ")
            times.append(float(columns[0]))
            if parameter:
               values.append(float(columns[int(parameter)]))
            else:
                positionxs.append(float(columns[1]))
                positionys.append(float(columns[2]))
                positionzs.append(float(columns[3]))
                directions.append(float(columns[4]))
    tpoints = np.array(times)
    if parameter:
        vpoints = np.array(values)
        plt.plot(tpoints, vpoints, label=f"Column {parameter}")
    else:
        xpoints = np.array(positionxs)
        ypoints = np.array(positionys)
        zpoints = np.array(positionzs)
        dpoints = np.array(directions)
        plt.plot(tpoints, xpoints, label="Position X")
        plt.plot(tpoints, ypoints, label="Position Y")
        plt.plot(tpoints, zpoints, label="Position Z")
        plt.plot(tpoints, dpoints, label="Direction")
    plt.legend()
    plt.show()
    return True

def _plottv(filePath, label=""):
    times = []
    values = []
    with open(filePath, 'r') as f:
        data = f.read()
        for line in data.splitlines():
            columns = line.split(" ")
            times.append(float(columns[0]))
            values.append(float(columns[1]))
    tpoints = np.array(times)
    vpoints = np.array(values)
    plt.plot(tpoints, vpoints, label=label)
    plt.legend()
    plt.show()
    return True

def _plotfps():
    return _plottv("fps.ofs", "FPS")

def _h(className):
    return smoothsource.generate("smoothsource/class_h_nons.smoothsource", {
        "className": className
    }, "include/" + className.lower() + ".h")

def _cpp(className):
    return smoothsource.generate("smoothsource/class_cpp.smoothsource", {
        "className": className,
        "namespace": ""
    }, "src/" + className.lower() + ".cpp")

def _dae(relpath):
    if relpath == None or len(relpath) == 0 or not os.path.exists(relpath):
        return False
    def find_all(ext, path):
        result = []
        for root, dirs, files in os.walk(path):
            for file in files:
                if file.endswith(ext):
                    result.append(os.path.join(root, file).replace('\\', '/'))
        return result   
    for path in find_all(".dae", relpath):
        content = ""
        with open(path, "r") as f:
            content = f.read()
            a = content.find("<bind_shape_matrix>") + len("<bind_shape_matrix>")
            b = content.find("</bind_shape_matrix>")
            content = content[0:a] + "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1" + content[b:]
        with open(path, "w") as f:
            f.write(content)
        
    return True

def _class(className):
    rval = _h(className)
    rval = rval and _cpp(className)
    return rval

def _glh(className, packageName):
    return smoothsource.generate("smoothsource/class_h.smoothsource", {
        "className": className,
        "namespace": "lithium",
    }, "dep/lithium/include/lithium/" + packageName + "/gl" + className.lower() + ".h")

def _glcpp(className, packageName):
    return smoothsource.generate("smoothsource/class_cpp.smoothsource", {
        "className": className,
        "namespace": "lithium",
    }, "dep/lithium/src/gl" + className.lower() + ".cpp")

def _glclass(className, packageName):
    rval = _glh(className, packageName)
    rval = rval and _glcpp(className, packageName)
    return rval

def _level(index, columns, rows):
     return smoothsource.generate("smoothsource/level.smoothsource", {
        "cols": columns,
        "rows": rows,
        "rowList": [{"rowItem": (int(columns) * ". ")} for x in range(int(rows))]
    }, "assets/levels/level" + str(index) + ".txt")

def _myglh(className):
    return smoothsource.generate("smoothsource/class_h.smoothsource", {
        "className": className,
        "namespace": "mygl"
    }, "dep/fogl/include/gl" + className.lower() + ".h")

def _myglcpp(className):
    return smoothsource.generate("smoothsource/class_cpp.smoothsource", {
        "className": className,
        "namespace": "mygl::", # should be solved without :: by using conditions
        "fileNamePrefix": "gl"
    }, "dep/fogl/src/gl" + className.lower() + ".cpp")

def _myglclass(className):
    rval = _myglh(className)
    rval = rval and _myglcpp(className)
    return rval
