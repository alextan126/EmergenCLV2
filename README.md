# EmergenCLV2: Semantic Gap Bench

## Goal

Moving to Level 2 is where the translation stops being a syntax exercise and becomes a test of architectural intent. In Level 1, there is usually a 1:1 mapping between a C concept and a Rust concept. In Level 2 (The Semantic Gap), that mapping breaks down.

C achieves flexibility through type erasure (`void*`), memory sharing through raw pointers, and concurrency through manual locks. Rust achieves these same goals through strict typing (Traits, Enums), ownership rules (`Box`, `Rc`), and thread-safe wrappers (`Arc`, `Mutex`). To pass Level 2, the LLM must understand what the C code is trying to achieve and completely redesign how it achieves it.

---

## 1. Polymorphism & Type Erasure

* **The C Paradigm:** Using `void*` and function pointers to handle multiple data types or build generic data structures.
* **The Rust Goal:** Utilizing Enums (for closed sets of types), Generics `<T>` (for compile-time monomorphization), or Trait Objects `Box<dyn Trait>` (for dynamic dispatch).

### Test Cases

* **Test Case 21: Heterogeneous Array**
  * **Description:** A C array holding different structs using a tagged union and `void*` payloads.
  * **Translation Goal:** An array of Rust enum variants containing the specific struct data.

* **Test Case 22: Generic Event Dispatcher**
  * **Description:** A C publisher/subscriber model where callbacks are registered as `void (*func)(void* user_data)` along with a `void*` context pointer.
  * **Translation Goal:** Using closures like `Box<dyn FnMut()>` or defining a specific `Handler` trait.

### True/False Judge Criteria

* **Safe Dispatch (Q21, Q22):** The code replaces `void*` payloads with Enums or Trait objects, completely avoiding unsafe pointer casting. *(True/False)*
* **Closure Support (Q22):** The event system utilizes Rust closures (`Fn`, `FnMut`) instead of requiring the caller to manually manage state via a generic context pointer. *(True/False)*

---

## 2. Concurrency & Shared State

* **The C Paradigm:** Global or heap-allocated state manipulated across threads using `pthread_mutex_t`, often relying on the programmer to remember to lock/unlock.
* **The Rust Goal:** Enforcing "Fearless Concurrency" by binding the data directly to the lock using `Mutex<T>` or `RwLock<T>`, and managing cross-thread lifecycles with `Arc<T>`.

### Test Cases

* **Test Case 23: Shared In-Memory Cache**
  * **Description:** A C hash map accessed by multiple threads, using a single global mutex and returning pointers to the cached data.
  * **Translation Goal:** `Arc<RwLock<HashMap<K, V>>>` where readers get safe, concurrent access.

* **Test Case 24: Producer-Consumer Queue**
  * **Description:** A C queue using condition variables (`pthread_cond_t`) and manual locking to push/pop tasks.
  * **Translation Goal:** Using `std::sync::mpsc` channels or a safely wrapped `Mutex<VecDeque<T>>` with `Condvar`.

### True/False Judge Criteria

* **Data Binding (Q23, Q24):** The shared data is explicitly wrapped inside the synchronization primitive (e.g., `Mutex<HashMap>`) rather than keeping the lock and the data structurally separate. *(True/False)*
* **Send/Sync Compliance (Q24):** The implementation utilizes `Arc` to share ownership across threads rather than passing raw pointers or attempting to share `Rc` across thread boundaries. *(True/False)*

---

## 3. Complex Lifetimes & Graph-like Ownership

* **The C Paradigm:** Structs that point to each other, parent-child pointers, or trees with nodes that maintain a pointer back to the root.
* **The Rust Goal:** Resolving borrow-checker conflicts without resorting to `unsafe`. This usually requires structural redesigns.

### Test Cases

* **Test Case 25: Parent-Child Tree Node**
  * **Description:** A C tree where a child node has a `struct Node*` parent pointer, used to traverse back up the tree.
  * **Translation Goal:** Using `Rc<RefCell<Node>>` for children and `Weak<RefCell<Node>>` for the parent pointer to prevent memory leaks, OR restructuring to use an Arena (vector of nodes) and indices.

* **Test Case 26: Opaque Handles / API Boundaries**
  * **Description:** A C library exposing a `struct Context`; in the header, passing opaque pointers to the user, who passes them back to library functions.
  * **Translation Goal:** Using an opaque Rust struct with privacy modifiers (`pub struct Context { internal: Inner }`) and implementing methods directly on it.

### True/False Judge Criteria

* **Weak References (Q25):** The code correctly identifies the cyclical nature of parent/child pointers and utilizes `std::rc::Weak` (or `std::sync::Weak`) to break the reference cycle. *(True/False)*
* **Encapsulation (Q26):** The API exposes a safe, opaque struct with associated methods (`impl Context`) rather than forcing the caller to handle raw pointers or `Box::into_raw`. *(True/False)*

---

## 4. Custom Memory Management

* **The C Paradigm:** Pre-allocating a massive block of memory with `malloc` and manually slicing it up for smaller objects to avoid fragmentation (a custom memory pool or arena).
* **The Rust Goal:** Recognizing the intent of the arena and utilizing lifetimes (`'a`) to tie the allocated objects to the lifetime of the pool, or using standard vectors if the custom allocator is redundant in Rust.

### Test Cases

* **Test Case 27: Custom Bump Allocator**
  * **Description:** A C struct managing a `char buffer[1024]` and a `size_t` offset, handing out `void*` pointers to different offsets.
  * **Translation Goal:** Returning typed mutable references (`&'a mut T`) tied to the lifetime of the allocator struct, preventing the caller from holding onto the memory after the allocator is cleared.

### True/False Judge Criteria

* **Lifetime Enforcement (Q27):** The allocator returns references tied to explicit lifetimes (e.g., `<'a>`) ensuring that the compiler prevents use-after-free bugs when the arena is reset. *(True/False)*
* **Alignment Safety (Q27):** The Rust implementation properly handles memory alignment requirements (e.g., using `std::alloc::Layout`) rather than blindly advancing byte offsets like a naive C bump allocator. *(True/False)*
