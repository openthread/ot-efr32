# Vendor SLC extensions

This is an example implementation of an SLC extension. SLC extensions allow vendors to create components, projects, etc. that are specific to their needs.

In this example, a new [component](https://siliconlabs.github.io/slc-specification/1.0/format/component/) called [`vendor_diag`](./component/vendor_diag.slcc) was created. The component pulls in [`src/diag.c`](./src/diag.c) from the extension's sources, which adds the `diag vendorversion` CLI command.

## Generate CMake libraries from an .slcp project

To generate the `.slcp` projects included in `configuration.yml`, just run

```bash
<path-to-vendor-extension>/script/generate
```

In this example, the command would look like

```bash
~/ot-efr32# $ examples/example_vendor_slc_extension/script/generate
```

This will generate the CMake files and the `autogen` and `config` directories for each platform project in `generated_projects/`.
These files can be committed so that they don't need to be generated every build.

## Building

Simply run the build script to start a build.

```bash
<path-to-vendor-extension>/script/build
```
