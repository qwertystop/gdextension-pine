# GDExtension-PINE

This project is a Godot interface for [PINE, a Protocol for the Instrumentation of Emulators](https://github.com/GovanifY/pine). It follows the standard [GDExtension template](https://github.com/godotengine/godot-cpp-template).

Compile with the standard tooling for GDExtension, `scons`. Note that, as the reference implementation of PINE is capable of raising exceptions, the minimal compilation command is `scons disable_exceptions=no`. See the GDExtension template docs for further details.

For a sample project using this, see https://github.com/qwertystop/EXEC_OVERLANG.
