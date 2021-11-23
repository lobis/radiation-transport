# How to run

```
docker run -d --cap-add sys_ptrace -p 2222:22 --name clion_remote_env ghcr.io/lobis/root-geant4-garfieldpp-clion:latest
```

Then connect as a remote host (at `0.0.0.0`, not `localhost`, if running locally) with username `user` and password `password`.
