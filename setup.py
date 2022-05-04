from distutils.core import setup, Extension

def main():
    setup(name="simple_graphs",
          version="1.0.0",
          description="my simple graph module",
          author="Marcin Malinowski",
          author_email="asdasd@gmail.com",
          ext_modules=[Extension("simple_graphs", ["simple_graphs.c"])])

if __name__ == "__main__":
    main()
