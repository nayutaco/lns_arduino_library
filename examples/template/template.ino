#include <LnShield.h>


namespace {
  LnShield sLn;       //Lightning Shield instance
}


//////////////////////////////////////////////////////////////

static void callbackChangeStatus(LnShield::Status_t status)
{
  if (status == LnShield::STATUS_NORMAL) {
    //ToDo: status 
  } else {
    //
  }
}


// @param [in] amountMsat amount after changed
static void callbackChangeMsat(uint64_t amountMsat)
{
  //ToDo: change local amount
}


static void callbackError(LnShield::Err_t err)
{
  //ToDo: LN shield detect error(not return)
}

//////////////////////////////////////////////////////////////

void setup() {
  //something initialize...

  sLn.init();
  sLn.eventInit(callbackChangeStatus, callbackChangeMsat, callbackError);
}


void loop() {
  //ToDo: catch event something to create invoice
  if (somethingDetect) {
    uint64_t requestAmount = xxxx;
    char description[20] = "description";
    if (sLn.cmdInvoice(requestAmount, description) == LnShield::ENONE) {
      //success request
      //  it takes 6 seconds to display QRcode to ePaper.
    } else {
      //fail request
    }
  }
  sLn.eventPoll();

  delay(100);
}
