Overview
========

D-Pipe is pipeline-based data processing framework for C++.
It allows an application to define custom processing elements and connect them in series.

Actors
------

* The D-Pipe framework, also referred simply as the framework.

* The application using the D-Pipe framework.

Naming conventions
------------------

* The basic transfer unit flowing inside the pipeline is a frame, _i.e._ a self-consistent chunk of data.
  For instance, this could be a packet acquired from a sensor or a probe.
  The actual content and format of the frame may change along the pipeline.
  From the application point of view, a frame is an instance of a class, allocated with a specific framework-defined mechanism.

* A pipeline element is an object that can produce, consume, or transform a frame.

* A pipeline is a series of such elements connected together.
  The output frames of one element are the input of the next one (or ones).

Requirements
------------

* The framework shall provide a way for the application to implement the following three kind of pipeline elements.
  - **Source**: an element that produces frames.
  - **Sink**: an element that consumes frames.
  - **Filter**: an element that transforms frames, _i.e._, it is both a source and a sink.
    For every input frame, the transformation can generate an arbitrary number of output frames, including none.

* The application shall then be able to link the elements together to create a pipeling.

* The framework shall guarantee the compile-time type-safety of the frames flowing through the pipeline.
  It shall not be possible to connect together an element to another expecting a different frame type.

* The application shall be able to allocate a frame of arbitrary type in any element of the pipeline.

* The application shall not be required to manually deallocate any frame.

* The framework shall provide a default mean to allocate data frames.
  The default allocation mechanism should be tuned for high frame rates.

* The application shall have the possibility to define its own underlying allocator.

* Frames shall flow through the pipeline without the need of deep-copying their content.
  In this way, the data transfer overhead is independent of the frame size.

* Each element of the pipeline shall push processed data into the following element(s) via a synchronous connection (a function call).
  Asynchronicity is still possible by decoupling the reception and the processing of the input data (see the Decoupler element).

* Each element of the pipeline shall be able to tell the subsequent element (if any) to flush its data.
  In this way, any pending operation gets the chance to be completed when the frame flow ends.

* The framework shall provide the following ready-to-use basic filters.
  - **Decoupler**: decouples two connected elements, so that the producer can push multiple frames without waiting for the consumer to finish its processing.
    It can also be used to multiplex frames coming from different sources.
  - **Splitter**: a special kind of filter that distributes frames to more than one sink.
  - **Switch**: either transparently passes on all frames or drops them based on an application-controllable flag.

* The framework shall provide a way to report pipeline element errors to the application.

* The framework shall provide a way to pass configuration parameters to the elements at run-time.

Example use case
----------------

In the context of predictive maintenance in industrial machinery, an application may use a data
pipeline for vibration monitoring.

* Acquire high-frequency vibration data from multiple accelerometers mounted on various points of a
  machine.
* Decouple the acquisition procedure from the rest of the pipeline.
* Apply calibrations.
* Split all frames into two branches.
* First branch:
  - Filter data basing on specific signatures or thresholds;
  - Decouple the pipeline up to here from the rest of the pipeline;
  - Store the filtered vibration data to file for logging purposes.
* Second branch:
  - Aggregate the high-frequency data to reduce the frame rate to about 25 fps;
  - Display real-time vibration patterns, frequency spectra, or RMS values on a graphical user interface.

Design
------

### Data frames

New frames are created inside a reference-counted smart pointer (`std::shared_ptr`).
After being filled and before being pushed into the pipeline, a frame is wrapped in a box that guarantees immutability.
In this way, frames can be safely passed by reference through parallel processing branches.
If an element needs to modify a frame, it must first deep-copy it in a new frame.

### Allocation

The default allocation mechanism shall be based on a memory pool, thus allowing for allocation with constant execution time.

The allocation mechanism is represented by an allocator class, used in the library as a template argument.

### Pipeline

Each processing element (source, sink, filter) is a template class that takes as arguments the input and output frame types.
Correct linking between the elements is thus enforced by type-checking at compile time.

The pipeline elements are actually implemented using a the decorator pattern.
The starting component is the sink, which exposes a method to push in frames.
Multiple filters can than be stacked on top of the sink to add processing steps (decorate), yet keeping the same interface.
