module Effect.Async
  ( Async
  , runA
  , waitA
  , liftEffectA
  , parTraverseA
  ) where

import Prelude
import Effect (Effect)

instance asyncFunctor :: Functor Async where
  map f x = bindA x (pureA <<< f)

instance asyncApply :: Apply Async where
  apply af aa = bindA aa (\a -> map (\f -> f a) af)

instance asyncApplicative :: Applicative Async where
  pure a = pureA a

instance asyncBind :: Bind Async where
  bind x f = bindA x f

instance asyncMonad :: Monad Async
  
foreign import data Async :: Type -> Type

foreign import pureA :: forall a. a -> Async a

foreign import liftEffectA :: forall a. Effect a -> Async a

foreign import bindA :: forall a b. Async a -> (a -> Async b) -> Async b

foreign import parTraverseA :: forall a b. Array (Async a) -> (a -> Async b) -> Async (Array b)

foreign import runA :: forall a. Async a -> Effect a

foreign import waitA :: Int -> Async Unit
 