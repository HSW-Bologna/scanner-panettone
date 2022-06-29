import os
import multiprocessing
from pathlib import Path
import tools.meta.csv2carray as csv2carray
import platform

#
# FUNCTIONS
#


def PhonyTargets(
    target,
    action,
    depends,
    env=None,
):
    # Creates a Phony target
    if not env:
        env = DefaultEnvironment()
    t = env.Alias(target, depends, action)
    env.AlwaysBuild(t)


#
# BUILD CONFIGURATION
#

MINGW = platform.system() == "Windows"
PROGRAM = "simulated.exe"
MAIN = "main"
ASSETS = "assets"
COMPONENTS = "components"
LVGL = f"{COMPONENTS}/lvgl"
LIBLIGHTMODBUS = f"{COMPONENTS}/liblightmodbus"

PARMAC_DESCRIPTIONS = f"{MAIN}/model/descriptions"
STRING_TRANSLATIONS = f"{MAIN}/view/intl"

TRANSLATIONS = [
    {
        "res": [f"{PARMAC_DESCRIPTIONS}/AUTOGEN_FILE_parmac.c", f"{PARMAC_DESCRIPTIONS}/AUTOGEN_FILE_parmac.h"],
        "input": f"{ASSETS}/translations/parmac",
        "output": PARMAC_DESCRIPTIONS,
    },
    {
        "res": [f"{STRING_TRANSLATIONS}/AUTOGEN_FILE_strings.c", f"{STRING_TRANSLATIONS}/AUTOGEN_FILE_strings.h"],
        "input": f"{ASSETS}/translations/strings",
        "output": STRING_TRANSLATIONS
    },
]


CFLAGS = [
    "-Wall",
    "-Wextra",
    "-Wno-unused-function",
    "-g",
    "-O0",
    "-DPC_SIMULATOR",
    "-DLV_CONF_INCLUDE_SIMPLE",
    "-DLV_LVGL_H_INCLUDE_SIMPLE",
    '-DGEL_PARAMETER_CONFIGURATION_HEADER="\\"gel_conf.h\\""',
    '-DGEL_PAGEMANAGER_CONFIGURATION_HEADER="\\"gel_conf.h\\""',
    "-Wno-unused-parameter",
    "-static-libgcc",
    "-static-libstdc++",
]
LDLIBS = ["-lmingw32", "-lSDL2main", "-lSDL2"] if MINGW else ["-lSDL2"]
SDLPATH = ARGUMENTS.get('sdl', None)
LIBPATH = [os.path.join(SDLPATH, 'lib')] if SDLPATH else []

CPPPATH = [
    f"#{COMPONENTS}", f"#{LVGL}/src", f"#{LVGL}", f"#{MAIN}",
    f"#{MAIN}/config", f"#{MAIN}/config/mbconf", f"#{MAIN}/simulator", f"#{COMPONENTS}/gel/generic_embedded_libs",
    f"{LIBLIGHTMODBUS}/include",
    "simulator",
]

if SDLPATH:
    CPPPATH.append(os.path.join(SDLPATH, 'include'))

#
# MAIN
#


def main():
    num_cpu = multiprocessing.cpu_count()
    SetOption('num_jobs', num_cpu)
    print("Running with -j {}".format(GetOption('num_jobs')))

    env_options = {
        "ENV": os.environ,
        "CC": ARGUMENTS.get('cc', 'gcc'),
        "ENV": os.environ,
        "CPPPATH": CPPPATH,
        'CPPDEFINES': [],
        "CCFLAGS": CFLAGS,
        "LIBS": LDLIBS,
    }

    env = Environment(**env_options, tools=['mingw'] if MINGW else None)
    env.Tool('compilation_db')

    translations = []
    for translation in TRANSLATIONS:
        def operation(t):
            return lambda target, source, env: csv2carray.main(t["input"], t["output"])

        env.Command(
            translation["res"], Glob(f"{translation['input']}/*.csv"), operation(translation))
        translations += translation["res"]

    gel_env = env
    gel_selected = ["pagemanager", "collections", "parameter", "timer",
                    "data_structures", "keypad", "debounce", "crc", "wearleveling"]
    (gel, include) = SConscript(
        f'{COMPONENTS}/generic_embedded_libs/SConscript', exports=['gel_env', 'gel_selected'])
    env['CPPPATH'] += [include]

    i2c_env = env
    i2c_selected = ['temperature/SHT3', "eeprom/24LC16",
                    'temperature/SHT21', 'LTR559ALS', "posix"]
    (i2c, include) = SConscript(
        f'{COMPONENTS}/I2C/SConscript', exports=['i2c_env', 'i2c_selected'])
    env['CPPPATH'] += [include]

    modbus_sources = [str(filename) for filename in Path(
        f"{LIBLIGHTMODBUS}/src").rglob('*.c')]

    sources = [str(filename) for filename in Path('simulator').rglob('*.c')]
    sources += [str(filename) for filename in Path('main/utils').rglob('*.c')]
    sources += [str(filename) for filename in Path('main/view').rglob('*.c')]
    sources += [str(filename) for filename in Path('main/model').rglob('*.c')]
    sources += [str(filename)
                for filename in Path('main/controller').rglob('*.c')]
    sources += [str(filename)
                for filename in Path(f'{LVGL}/src').rglob('*.c')]
    sources += [Glob(f'{LVGL}/*.c')]
    #sources += [t for t in translations if t not in sources]
    # remove duplicates
    sources = list(dict.fromkeys([x for x in sources if x]))

    prog = env.Program(PROGRAM, sources + modbus_sources + i2c +
                       gel, LIBPATH=LIBPATH)
    PhonyTargets('run', os.path.join(".", PROGRAM), prog, env)
    #env.Alias("intl", translations)
    compileDB = env.CompilationDatabase('compile_commands.json')

    env.NoClean(translations)
    #env.Depends(prog, "intl")
    env.Depends(prog, compileDB)
    env.Default(prog)


main()
