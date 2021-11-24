# Build

```
CMAKE_CXX_STANDARD=17
ROOT_VERSION=v6-25-01
GEANT4_VERSION=v10.7.3

docker build --build-arg CMAKE_CXX_STANDARD=$CMAKE_CXX_STANDARD --build-arg ROOT_VERSION=$ROOT_VERSION --build-arg GEANT4_VERSION=$GEANT4_VERSION -t lobis/root-geant4-garfieldpp:cxx${CMAKE_CXX_STANDARD}_ROOT${ROOT_VERSION}_Geant4${GEANT4_VERSION} .
```

# Push

Make sure to authenticate to the corresponding registry.

```
docker push lobis/root-geant4-garfieldpp:cxx${CMAKE_CXX_STANDARD}_ROOT${ROOT_VERSION}_Geant4${GEANT4_VERSION}
```
