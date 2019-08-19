module Effect.Async
  ( Async
  , PAsync
  , runSynchronously
  , wait
  ) where

import Prelude

import Control.Parallel (class Parallel)
import Effect (Effect)
import Effect.Class (class MonadEffect)
import Partial.Unsafe (unsafePartial)
import Unsafe.Coerce (unsafeCoerce)

runSynchronously :: forall a . Async a -> Effect a
runSynchronously = runA

wait :: Int -> Async Unit
wait = waitA

data ForVal a b = Func (a -> b) | Value a

instance asyncFunctor :: Functor Async where
  map f x = bindA x (pureA <<< f)

instance asyncApply :: Apply Async where
  apply af aa = bindA aa (\a -> map (\f -> f a) af)

instance asyncApplicative :: Applicative Async where
  pure a = pureA a

instance asyncBind :: Bind Async where
  bind x f = bindA x f

instance asyncMonad :: Monad Async

instance asyncEffect :: MonadEffect Async where
  liftEffect x = liftEffectA x

instance parallelAsyncFunctor :: Functor PAsync where
  map f x = pure f <*> x

instance parallelAsyncApplicative :: Applicative PAsync where
  pure a = unsafeCoerce $ pureA a

instance parallelApply ::  Apply PAsync where
  apply af aa = 
    unsafeCoerce $ map (unsafePartial <<< \[Func f, Value a] -> f a) $ parForA [af', aa'] (identity)
    where 
      af' :: forall a b. Async (ForVal a b)
      af' = map Func $ unsafeCoerce af
      aa' :: forall a b. Async (ForVal a b)
      aa' = map Value $ unsafeCoerce aa

instance parallelAsync :: Parallel PAsync Async where
  parallel m = unsafeCoerce m
  sequential m = unsafeCoerce m
  
foreign import data Async :: Type -> Type

foreign import data PAsync :: Type -> Type

foreign import pureA :: forall a. a -> Async a

foreign import liftEffectA :: forall a. Effect a -> Async a

foreign import bindA :: forall a b. Async a -> (a -> Async b) -> Async b

foreign import parForA :: forall a b. Array a -> (a -> Async b) -> Async (Array b)

foreign import runA :: forall a. Async a -> Effect a

foreign import waitA :: Int -> Async Unit
 