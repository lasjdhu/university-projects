PROJECT = src/ipk25chat-client.csproj
TARGET = ipk25chat-client

.PHONY: all build publish test clean clean-tests

all: publish

build:
	dotnet build $(PROJECT) -c Release

publish:
	dotnet publish $(PROJECT) \
		-c Release \
		-o . \
		--self-contained true \
		-r linux-x64 \
		-p:PublishSingleFile=true \
		-p:DebugType=none \
		-p:AssemblyName=$(TARGET)

test:
	cd tests && dotnet test

clean:
	dotnet clean $(PROJECT)
	rm -rf $(TARGET) ./src/bin ./src/obj

clean-tests:
	dotnet clean tests
	rm -rf ./tests/bin ./tests/obj
