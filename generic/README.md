NXT3 Hardware Abstraction Layer
===============================

This module replaces the NXT driver layer (d_*). It defines the interface
between platform implementations and the NXT VM core. It consists mainly
of C headers with function declarations. However, is also implements some
common functionality directly here.

Modules
-------

- miscellaneous
  - `hal_general` - system management stuff
  - `hal_timer` - timekeeping (clock_gettime etc.)
  - `hal_battery` - battery monitoring (voltage, current, capacity, ...)
- filesystem
  - `hal_errorcodes` - common error code definitions
  - `hal_filesystem` - the FS implementation itself
- brick ui
  - `hal_display` - framebuffer, frame draw only at the time
  - `hal_button` - brick buttons
  - `hal_led` - brick LEDs
  - `hal_sound` - beeps, melodies, u8 8000 kHz PCM
- peripherals
  - `hal_adc` - "analog" sensor link (OOP-style)
  - `hal_iic` - "lowspeed" sensor link (OOP-style)
  - `hal_motor` - configuring motor PWM + reading motor tacho
  - `hal_pnp` - common interface to device hotplug handling
- external comms
  - `hal_bt` - bluetooth management (missing)
  - `hal_rs485` - "hispeed" sensor/brick link (stub only)
  - `hal_usb` - implementation of a USB channel to a host PC

Abstraction styles
------------------

There are two predominant styles in the HAL:

 - Global singletons using reference counting for their lifetime management.

   These are typically used for things that do not change, e.g. battery,
   filesystem and so on. Taking a different approach would IMHO introduce
   unnecessary complexity.


   Reference counting is implemented via the following two functions:
   ```c
   bool Hal_xxx_RefAdd(void);
   bool Hal_xxx_RefDel(void);
   ```

   Both have a `bool` return type. `*RefAdd()` is expected to be able to
   fail. In such case, it should clean up any allocated resources.
   `*RefDel()` is not expected to fail in a typical sense -- usually
   the only failure model is deleting a module with refcount == 0.

   The functionality itself is then implemented by other functions.
   ```c
   bool Hal_Tardis_TimeTravel(struct timespec where);
   ```

 - Attachable objects with implementation-defined lifetime handling.

   This style is loosely inspired by the OOP-style patterns used in the
   Linux kernel. It is used primarily for things that require flexibility
   in terms of used implementation. One such example are sensor adapters
   which need to change depending on the sensor currently plugged in.

   Because hardware discovery and other stuff is in the hands of the HAL
   implementation, no initialization interfaces are available to the NXT
   core. Instead, in order to register a "driver" for a "port", the HAL
   is expected to perform a reverse call to the NXT core. Such functions
   would have a signature like this:
   ```c
   bool Hal_xxxHost_Attach(hal_xxx_dev_t *dev);
   bool Hal_xxxHost_Detach(hal_xxx_dev_t *dev);
   ```

   To implement polymorphic behaviour, a `xxx_dev_t` and `xxx_ops_t` structs
   are defined. The `xxx_dev_t` struct contains data used by the common HAL
   code (see below) and a pointer to a `xxx_ops_t` structure. The `xxx_ops_t`
   structure contains pointers to device-specific function implementations.
   ```c
   typedef struct hal_xxx_dev hal_xxx_dev_t;
   typedef struct hal_xxx_ops hal_xxx_ops_t;

   struct hal_xxx_dev {
       const hal_xxx_ops_t *ops;
       int port;
       char random_data;
   };

   struct hal_xxx_ops {
       bool (*do_foo)(hal_xxx_dev_t *dev, int answer_to_life_the_universe_and_everything);
       void (*do_bar)(hal_xxx_dev_t *dev, bool really);
   };
   ```

   One important thing to notice is the first argument of the ops
   functions - the device itself. This is equivalent to the "this" pointer
   in C++, just in C it has to be passed manually.

   The core is not expected to call functions in `hal_xxx_ops` directly,
   though. Instead, a set of wrapper functions is implemented in this
   module. These functions can for example check for null, sanitize
   arguments, adapt different interfaces etc. It also avoids having to
   write cals like `dev->ops->do_foo(dev, 42)`.
   ```c
   bool Hal_xxx_DoFoo(hal_xxx_dev_t *dev, int answer_to_life_the_universe_and_everything);
   void Hal_xxx_DoBar(hal_xxx_dev_t *dev, bool really);
   ```

   You may ask, how does the driver access its own private data? Do they
   have to be defined globally? Luckily, this is not the case. The key to
   this is the `container_of` macro inspired by the Linux kernel. It allows
   you to get pointer to a structure containing another structure as a member
   if you have pointer to the child structure. This way, you can implement
   private data through the parent structure:
   ```c
   typedef struct cool_device_t {
        hal_xxx_dev_t link;
        int reality;
   };

   #define to_cooldev(dev) container_of(dev, cool_device_t, link)

   bool cool_device_foo(hal_xxx_dev_t *dev, int the_answer) {
       cool_device_t *this = to_cooldev(dev);
       return the_answer == this->reality;
   }

   TBD: ops initialization

License
-------

This module is licensed under the terms of the MIT License, see
[LICENSE.txt](LICENSE.txt).
