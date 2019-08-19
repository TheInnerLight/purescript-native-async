module Effect.Async where

import Prelude

import Effect (Effect)
  
foreign import data Async :: Type -> Type

foreign import pureA :: forall a. a -> Async a

foreign import bindA :: forall a b. Async a -> (a -> Async b) -> Async b

foreign import runA :: forall a. Async a -> Effect a

foreign import waitA :: Async Unit
