class IpcInterface {
 public:
  static IpcInterface *Create(int queue_limit);
  virtual ~IpcInterface() {}

  virtual void Close() = 0;

  virtual bool InitializeWasher() = 0;
  // Send a message from Washer to the Dryer (put a dish on the table). 
  // Should block if 'queue_limit' is reached until Dryer gets the message
  virtual bool SendMessage(char message) = 0;

  virtual bool InitializeDryer() = 0;
  // Dryer - get a message (take a dish from the table) 
  // Unblocks Washer if it was blocked in SendMessage 
  virtual bool GetMessage(char *message) = 0;
};
