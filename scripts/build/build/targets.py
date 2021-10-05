# Copyright (c) 2021 Project CHIP Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os

from builders.android import AndroidBoard, AndroidBuilder
from builders.efr32 import Efr32Builder, Efr32App, Efr32Board
from builders.esp32 import Esp32Builder, Esp32Board, Esp32App
from builders.host import HostBuilder, HostApp, HostBoard
from builders.nrf import NrfApp, NrfBoard, NrfConnectBuilder
from builders.qpg import QpgBuilder
from builders.infineon import InfineonBuilder, InfineonApp, InfineonBoard
from builders.telink import TelinkApp, TelinkBoard, TelinkBuilder
from builders.tizen import TizenApp, TizenBoard, TizenBuilder


class Target:
    """Represents a build target:

        Has a name identifier plus parameters on how to build it (what
        builder class to use and what arguments are required to produce
        the specified build)
    """

    def __init__(self, name, builder_class, **kwargs):
        self.name = name
        self.builder_class = builder_class
        self.create_kw_args = kwargs

    def Extend(self, suffix, **kargs):
        """Creates a clone of the current object extending its build parameters.

        Arguments:
           suffix: appended with a "-" as separator to the clone name
           **kargs: arguments needed to produce the new build variant
        """
        clone = Target(self.name, self.builder_class,
                       **self.create_kw_args.copy())
        clone.name += "-" + suffix
        clone.create_kw_args.update(kargs)
        return clone

    def Create(self, runner, repository_path: str, output_prefix: str, enable_flashbundle: bool):
        builder = self.builder_class(
            repository_path, runner=runner, **self.create_kw_args)

        builder.identifier = self.name
        builder.output_dir = os.path.join(output_prefix, self.name)
        builder.enable_flashbundle(enable_flashbundle)

        return builder


def HostTargets():
    target = Target(HostBoard.NATIVE.PlatformName(), HostBuilder)
    targets = [
        target.Extend(HostBoard.NATIVE.BoardName(), board=HostBoard.NATIVE)
    ]

    # x64 linux  supports cross compile
    if (HostBoard.NATIVE.PlatformName() == 'linux') and (
            HostBoard.NATIVE.BoardName() != HostBoard.ARM64.BoardName()):
        targets.append(target.Extend('arm64', board=HostBoard.ARM64))

    for target in targets:
        yield target.Extend('all-clusters', app=HostApp.ALL_CLUSTERS)
        yield target.Extend('chip-tool', app=HostApp.CHIP_TOOL)
        yield target.Extend('thermostat', app=HostApp.THERMOSTAT)


def Esp32Targets():
    target = Target('esp32', Esp32Builder)

    yield target.Extend('m5stack-all-clusters', board=Esp32Board.M5Stack, app=Esp32App.ALL_CLUSTERS)
    yield target.Extend('c3devkit-all-clusters', board=Esp32Board.C3DevKit, app=Esp32App.ALL_CLUSTERS)

    devkitc = target.Extend('devkitc', board=Esp32Board.DevKitC)

    yield devkitc.Extend('all-clusters', app=Esp32App.ALL_CLUSTERS)
    yield devkitc.Extend('shell', app=Esp32App.SHELL)
    yield devkitc.Extend('lock', app=Esp32App.LOCK)
    yield devkitc.Extend('bridge', app=Esp32App.BRIDGE)
    yield devkitc.Extend('temperature-measurement', app=Esp32App.TEMPERATURE_MEASUREMENT)


def Efr32Targets():
    target = Target('efr32-brd4161a-light', Efr32Builder,
                    board=Efr32Board.BRD4161A)

    targets = [
        target.Extend('light', app=Efr32App.LIGHT),
        target.Extend('lock', app=Efr32App.LOCK),
        target.Extend('window-covering', app=Efr32App.WINDOW_COVERING),
    ]

    for target in targets:
        yield target
        yield target.Extend('rpc', enable_rpcs=True)


def NrfTargets():
    target = Target('nrf', NrfConnectBuilder)

    targets = [
        target.Extend('nrf5340', board=NrfBoard.NRF5340),
        target.Extend('nrf52840', board=NrfBoard.NRF52840),
    ]

    for target in targets:
        yield target.Extend('lock', app=NrfApp.LOCK)
        yield target.Extend('light', app=NrfApp.LIGHT)
        yield target.Extend('shell', app=NrfApp.SHELL)
        yield target.Extend('pump', app=NrfApp.PUMP)
        yield target.Extend('pump-controller', app=NrfApp.PUMP_CONTROLLER)


def AndroidTargets():
    target = Target('android', AndroidBuilder)

    yield target.Extend('arm-chip-tool', board=AndroidBoard.ARM)
    yield target.Extend('arm64-chip-tool', board=AndroidBoard.ARM64)
    yield target.Extend('x64-chip-tool', board=AndroidBoard.X64)
    yield target.Extend('x86-chip-tool', board=AndroidBoard.X86)


ALL = []

target_generators = [
    HostTargets(),
    Esp32Targets(),
    Efr32Targets(),
    NrfTargets(),
    AndroidTargets(),
]

for generator in target_generators:
    for target in generator:
        ALL.append(target)

# Simple targets added one by one
ALL.append(Target('qpg-qpg6100-lock', QpgBuilder))
ALL.append(Target('telink-tlsr9518adk80d-light', TelinkBuilder,
                  board=TelinkBoard.TLSR9518ADK80D, app=TelinkApp.LIGHT))
ALL.append(Target('infineon-p6-lock', InfineonBuilder,
                  board=InfineonBoard.P6BOARD, app=InfineonApp.LOCK))
ALL.append(Target('tizen-arm-light', TizenBuilder,
                  board=TizenBoard.ARM, app=TizenApp.LIGHT))

# have a consistent order overall
ALL.sort(key=lambda t: t.name)
