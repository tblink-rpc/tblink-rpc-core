
- Language-independent documentation -- key concepts
  - Endpoint
  - Interface Type
  - Method Type
  - Interface Instance
  - Interactions
    - build, connect, run
    - blocking and non-blocking interactions
    - Time providers and time clients
  - ...
  
- Layers
  - Datalink
  - Transport (message)
  - API
  - User-facade
  
- RPC messaging protocol documentation

- C++ API documentation
  - IParamVal lifecycle
- Python API documentation
- ... API documentation

- Developer documentation
  -

# Types of call targets
- Blocking -- req/rsp pairs for call and return
- Non-Blocking -- Single req/rsp pair for call and return
  - Response happens within request
  - Response separated from request by (typically delta) time

Different environments will handle non-blocking calls differently
- Multi-process socket: keep polling the socket until req/rsp even
- Single-process: keep running delta cycles until req/rsp even

# Startup-up procedure
- Build
  - Each endpoint publishes interface types and instances known
    to that endpoint. 
        
- Post-Build
  - At the end of the build phase, each endpoint:
    - Triggers the end-of-build callback. This may result in
      the creation of local interface types or instances based
      on what the peer endpoint has published. For example, 
      a BFM library may not require the user to create 
      instances on the testbench endpoint to match the simulator
      endpoint, but still wish to control instance creation.
    - Compares interface types and instances with same names
      and confirms that they are identical
    - Creates 'stub' local types and instances if a matching
      type/instance is not declared by the local endpoint
        - TODO: should this be optinal behavior?
        
- Connect
  - After the build phase is finished, user code can probe
    available types and instances. Typically, this is to
    acquire instances for later use, or to connect 
    event callback functions
    
- Post-Connect
  - After the connect phase is complete, both endpoint 
    peers are read to proceed with execution.
    
    