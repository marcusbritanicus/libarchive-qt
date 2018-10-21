TEMPLATE = subdirs

# Directories
SUBDIRS += shared static example

shared.file = lib/shared.pro

static.file = lib/static.pro
static.depends = shared

example.depends = static
example.file = example/example.pro
