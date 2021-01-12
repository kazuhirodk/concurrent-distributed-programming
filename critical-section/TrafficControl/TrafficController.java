public class TrafficController {
  // Declare config variables
  private final static int CONSECUTIVE_TURNS = 3;
  private final static String LEFT = "left";
  private final static String RIGHT = "right";

  // Declare initial statuses
  private String fluxDirection = LEFT;
  private boolean leftCarIsWaitingToEnter = false;
  private boolean rightCarIsWaitingToEnter = false;
  private boolean changingDirection = false;
  private int carsOnBridgeCount = 0;
  private int consecutiveDirectionCount = 0;

  private Boolean isFreePassToLeftCars() {
    return (!rightCarIsWaitingToEnter) && carsOnBridgeCount == 0;
  }

  private Boolean isFreePassToRightCars() {
    return (!leftCarIsWaitingToEnter) && carsOnBridgeCount == 0;
  }

  private Boolean reachConsecutiveLimit() {
    return (consecutiveDirectionCount >= CONSECUTIVE_TURNS);
  }

  private Boolean leftNeedsToStop() {
    return (reachConsecutiveLimit() && rightCarIsWaitingToEnter);
  }

  private Boolean rightNeedsToStop() {
    return (reachConsecutiveLimit() && leftCarIsWaitingToEnter);
  }

  private Boolean hasLeftCarsNextOnQueue() {
    return carsOnBridgeCount == 0 && leftCarIsWaitingToEnter;
  }

  private Boolean hasRightCarsNextOnQueue() {
    return carsOnBridgeCount == 0 && rightCarIsWaitingToEnter;
  }

  private void setLeftToWaitingStatus() {
    sinalizeDirectionChange();
    leftCarIsWaitingToEnter = true;
  }

  private void setLeftToFreeStatus() {
    leftCarIsWaitingToEnter = false;
  }

  private void setRightToWaitingStatus() {
    sinalizeDirectionChange();
    rightCarIsWaitingToEnter = true;
  }

  private void setRightToFreeStatus() {
    rightCarIsWaitingToEnter = false;
  }

  private void sinalizeDirectionChange() {
    changingDirection = reachConsecutiveLimit();
  }

  private void initiateTrafficToTheRight() {
    fluxDirection = RIGHT;
    consecutiveDirectionCount = 0;
  }

  private void initiateTrafficToTheLeft() {
    fluxDirection = LEFT;
    consecutiveDirectionCount = 0;
  }

  private void incrementCarsCountOnBridge() {
    carsOnBridgeCount++;
    consecutiveDirectionCount++;
  }

  private void decrementCarsCountOnBridge() {
    carsOnBridgeCount--;
  }

  public synchronized void enterLeft() {
    if (isFreePassToLeftCars()) {
      initiateTrafficToTheRight();
    } else {
      while (leftNeedsToStop() || fluxDirection == LEFT || changingDirection) {
        setLeftToWaitingStatus();

        try {
          wait();
        } catch (InterruptedException e) {}

        setLeftToFreeStatus();
      }
    }

    incrementCarsCountOnBridge();
  }

  public synchronized void enterRight() {
    if (isFreePassToRightCars()) {
      initiateTrafficToTheLeft();
    } else {
      while (rightNeedsToStop() || fluxDirection == RIGHT || changingDirection) {
        setRightToWaitingStatus();

        try {
          wait();
        } catch (InterruptedException e) {}

        setRightToFreeStatus();
      }
    }

    incrementCarsCountOnBridge();
  }

  public synchronized void leaveLeft() {
    decrementCarsCountOnBridge();

    if (hasLeftCarsNextOnQueue()) {
      initiateTrafficToTheRight();
      changingDirection = false;

      notifyAll();
    }
  }

  public synchronized void leaveRight() {
    decrementCarsCountOnBridge();

    if (hasRightCarsNextOnQueue()) {
      initiateTrafficToTheLeft();
      changingDirection = false;

      notifyAll();
    }
  }
}
