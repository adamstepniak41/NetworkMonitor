FROM gcc:4.9
RUN apt update && apt install -y python-pip && pip install setuptools -U && pip install conan
